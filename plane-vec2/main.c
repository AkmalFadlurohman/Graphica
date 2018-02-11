/*
    Program to print font direct to frame buffer.
    Assumption: use 32bpp screen
*/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "headers/VecLetter.h"
#include <math.h>
#define SCALE 1

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

char *fbp = 0;
int centerX = 0;
int fullY = 0;

int letterCount, letterHeight;
struct VecLetter** letters;

unsigned int rgbaToInt(int r, int g, int b, int a);
void drawPixel(int x, int y, unsigned int color);
//void drawPixelWithScale(int x, int y, unsigned int color, int scale);
void clear(int color);
int isValidPoint(int x, int y);
//int isValidPointScale(int x, int y, int scale);
unsigned int getPixelColor(int x, int y);
void drawLineLow(double x0, double y0, double x1, double y1, unsigned int color);
//void drawLineLowWithScale(double x0, double y0, double x1, double y1, unsigned int color, int scale);
void drawLineHigh(double x0, double y0, double x1, double y1, unsigned int color);
//void drawLineHighWithScale(double x0, double y0, double x1, double y1, unsigned int color, int scale);
//void drawLine(double x0, double y0, double x1, double y2, unsigned int color);
//void drawLineWithScale(double x0, double y0, double x1, double y2, unsigned int color, int Scale);
//void drawLaser(int x0, int y0, int dx, int dy, int scale, int* pt);
void drawLine_line(struct Line* line, unsigned int color, int offsetX, int offsetY);
void loadLetters(char* fileName);
int drawVector(char c, int x, int y, unsigned int border_color, unsigned int fill_color, float degree, int originX, int originY, float zoom);
void fillLetter(struct VecLetter *vecletter, unsigned int color, unsigned int boundaryColor, int offsetX, int offsetY);

int main() {

    int fbfd = 0;
    long int screensize = 0;

    // Open the file for reading and writing
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }
    printf("Detected display: %dx%d, %dbpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }

    //Render and start animation
    
    loadLetters("spec.txt");
    

    for (int i=0; i<vinfo.yres/17; i++) {
      printf("\n");
    }
    printf("\n");
    int f;
    int marginX = 150*SCALE;
    int posX = marginX;
    int posY = vinfo.yres/2;
    system("clear");
    float degree = 10;
    float wingDeg = 0;
    float zoom = 1;
    while (1) {
        clear(rgbaToInt(0,0,0,0));

        wingDeg+=10;
        if(posX < vinfo.xres/(1.75*SCALE) && zoom == 1){
            if(degree < 10){
                degree+=1;

            } else if(degree == 10){
                posX+=4;

            }else{
                degree = 10;
            }
        }
        if(posX >= vinfo.xres/(1.75*SCALE) && zoom < 4){
            if(degree > 0){
                degree-=1;
                posX+=5;

            } else if(degree == 0){
                zoom+= 0.025;

            }else{
                degree = 0;
            }
        }
        if(zoom >= 4 && posX >marginX){
            if(degree > -10){
                degree-=1;
                posX-=6;

            } else if(degree == -10){
                posX-=8;

            }else{
                degree = -10;
            }
      

        }
        if(posX < marginX){
            posX = marginX;
        }
        if(posX == marginX && zoom !=1){
            if(degree < 0){
                degree+=1;

            } else if(degree == 0){
                zoom-= 0.025;

            }else{
                degree = 0;
            }
        }
        if(zoom < 1){
            zoom =1;
        }
        drawVector('C', posX, posY, rgbaToInt(0,0,255,0), rgbaToInt(0,0,150,0), degree, 50, 50, zoom);
        drawVector('B', posX, posY, rgbaToInt(0,255,0,0), rgbaToInt(0,150,0,0), degree, 50, 50, zoom);
        drawVector('A', posX, posY, rgbaToInt(255,0,0,0), rgbaToInt(150,0,0,0), degree, 50, 50, zoom);
        drawVector('D', posX, posY, rgbaToInt(0,255,255,0), rgbaToInt(0,150,150,0),wingDeg, 50, 60, zoom);
         usleep(30000);
   }



    return 0;
}

void clear(int color){
    long int location;

    for(int x = 0; x < vinfo.xres;x++){
        for(int y = 0; y < vinfo.yres;y++){
             location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = color;
            *(fbp + location + 1) = color >> 8;
            *(fbp + location + 2) = color >> 16;
            *(fbp + location + 3) = color >> 24;
        }
    }
}
unsigned int rgbaToInt(int r, int g, int b, int a) {
    return a << 24 | r << 16 | g << 8 | b;
}

unsigned int getPixelColor(int x, int y) {
    long int location;
    x = x * SCALE;
    y = y * SCALE;
    location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) + (y + vinfo.yoffset) * finfo.line_length;
    unsigned int blue = *(fbp + location);
    unsigned int green = *(fbp + location + 1);
    unsigned int red = *(fbp + location + 2);
    unsigned int alpha = *(fbp + location + 3) ^ 0xffffff00;
    if(green > 127)
        green = green ^ 0xffffff00;
    if(red > 127)
        red = red ^ 0xffffff00;
    if(blue > 127)
        blue = blue ^ 0xffffff00;
    if(alpha > 127)
        alpha = alpha ^ 0xffffff00;

   return rgbaToInt(red, green, blue, alpha);
}

void drawPixel(int x, int y, unsigned int color) {
    long int location;
    int i = 0, j = 0;
    x = x*SCALE; y = y*SCALE;
    for (i = 0; i < SCALE; i++)
        for (j = 0; j < SCALE; j++) {
            location = (x+i+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+j+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = color;
            *(fbp + location + 1) = color >> 8;
            *(fbp + location + 2) = color >> 16;
            *(fbp + location + 3) = color >> 24;
        }
}

// void drawPixelWithScale(int x, int y, unsigned int color, int scale) {
//     long int location;
//     int i = 0, j = 0;
//     x = x*scale; y = y*scale;
//     for (i = 0; i < scale; i++)
//         for (j = 0; j < scale; j++) {
//             location = (x+i+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+j+vinfo.yoffset) * finfo.line_length;
//             *(fbp + location) = color;
//             *(fbp + location + 1) = color >> 8;
//             *(fbp + location + 2) = color >> 16;
//             *(fbp + location + 3) = color >> 24;
//         }
// }

int isValidPoint(int x, int y) {
    if (x >= 0 && x < vinfo.xres/SCALE && y >=0 && y < vinfo.yres/SCALE)
        return 1;

    return 0;
}

// int isValidPointScale(int x, int y, int scale) {
//     if (x >= 0 && x < vinfo.xres/scale && y >=0 && y < vinfo.yres/scale)
//         return 1;

//     return 0;
// }

void drawLineLow(double x0, double y0, double x1, double y1, unsigned int color) {
    double dx, dy, D;
    dx = x1 - x0;
    dy = y1 - y0;
    int yi = 1;
    if (dy < 0) {
        yi = -1;
        dy = -dy;
    }

    D = 2 * dy - dx;
    int y = y0;

    for (double x = x0; x <= x1; x++) {
        if (isValidPoint(x, y) == 0)
            return;
        drawPixel(x, y, color);
        if (D > 0) {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }

    
}

// void drawLineLowWithScale(double x0, double y0, double x1, double y1, unsigned int color, int scale) {
//     double dx, dy, D;
//     dx = x1 - x0;
//     dy = y1 - y0;
//     int yi = 1;
//     if (dy < 0) {
//         yi = -1;
//         dy = -dy;
//     }

//     D = 2 * dy - dx;
//     int y = y0;

//     for (double x = x0; x <= x1; x++) {
//         if (isValidPointScale(x, y, scale) == 0)
//             return;
//         drawPixelWithScale(x, y, color, scale);
//         if (D > 0) {
//             y += yi;
//             D -= 2 * dx;
//         }
//         D += 2 * dy;
//     }
// }

void drawLineHigh(double x0, double y0, double x1, double y1, unsigned int color) {
    double dx, dy, D;
    dx = x1 - x0;
    dy = y1 - y0;
    int xi = 1;

    if (dx < 0) {
        xi = -1;
        dx = -dx;
    }

    D = 2 * dx - dy;
    int x = x0;

    for (double y = y0; y <= y1; y++) {
        if (isValidPoint(x, y) == 0)
            return;
        drawPixel(x, y, color);
        if (D > 0) {
            x += xi;
            D -= 2 * dy;
        }

        D += 2 * dx;
    }
}

// void drawLineHighWithScale(double x0, double y0, double x1, double y1, unsigned int color, int scale) {
//     double dx, dy, D;
//     dx = x1 - x0;
//     dy = y1 - y0;
//     int xi = 1;

//     if (dx < 0) {
//         xi = -1;
//         dx = -dx;
//     }

//     D = 2 * dx - dy;
//     int x = x0;

//     for (double y = y0; y <= y1; y++) {
//         if (isValidPointScale(x, y, scale) == 0)
//             return;
//         drawPixelWithScale(x, y, color, scale);
//         if (D > 0) {
//             x += xi;
//             D -= 2 * dy;
//         }

//         D += 2 * dx;
//     }
// }

// void drawLine(double x0, double y0, double x1, double y1, unsigned int color) {
//     if (abs(y1 - y0) < abs(x1 - x0)) {
//         if (x0 > x1) {
//             drawLineLow(x1, y1, x0, y0, color);
//         } else {
//             drawLineLow(x0, y0, x1, y1, color);
//         }
//     } else {
//         if (y0 > y1) {
//             drawLineHigh(x1, y1, x0, y0, color);
//         } else {
//             drawLineHigh(x0, y0, x1, y1, color);
//         }
//     }
// }

void drawLine_line(struct Line* line, unsigned int color, int offsetX, int offsetY) {
    if (abs(line->points[1]->y - line->points[0]->y) < abs(line->points[1]->x - line->points[0]->x)) {
        if (line->points[0]->x > line->points[1]->x) {
            drawLineLow(line->points[1]->x + offsetX, line->points[1]->y + offsetY, line->points[0]->x + offsetX, line->points[0]->y + offsetY, color);
        } else {
            drawLineLow(line->points[0]->x + offsetX, line->points[0]->y + offsetY, line->points[1]->x + offsetX, line->points[1]->y + offsetY, color);
        }
    } else {
        if (line->points[0]->y > line->points[1]->y) {
            drawLineHigh(line->points[1]->x + offsetX, line->points[1]->y + offsetY, line->points[0]->x + offsetX, line->points[0]->y + offsetY, color);
        } else {
            drawLineHigh(line->points[0]->x + offsetX, line->points[0]->y + offsetY, line->points[1]->x + offsetX, line->points[1]->y + offsetY, color);
        }
    }
}

// void drawLineWithScale(double x0, double y0, double x1, double y1, unsigned int color, int scale) {
//     if (abs(y1 - y0) < abs(x1 - x0)) {
//         if (x0 > x1) {
//             drawLineLowWithScale(x1, y1, x0, y0, color, scale);
//         } else {
//             drawLineLowWithScale(x0, y0, x1, y1, color, scale);
//         }
//     } else {
//         if (y0 > y1) {
//             drawLineHighWithScale(x1, y1, x0, y0, color, scale);
//         } else {
//             drawLineHighWithScale(x0, y0, x1, y1, color, scale);
//         }
//     }
// }

// void drawLaser(int x0, int y0, int dx, int dy, int scale, int* pt) {
//     int t = *pt;
//     if (isValidPointScale(x0 + dx * t, y0 + dy * t, scale) == 0) {
//         t = 1; *pt = 1;
//     }

//     drawLineWithScale(x0, y0, x0 + dx * t, y0 + dy * t, rgbaToInt(0,0,0,0), scale);
//     x0 = x0 + dx * t;
//     y0 = y0 + dy * t;
//     drawLineWithScale(x0, y0, x0 + dx * t, y0 + dy * t, rgbaToInt(0,255,0,0), scale);
// }

void loadLetters(char* fileName) {
    FILE * specFile;
    size_t bufferSize = 80;
    char *buffer = malloc(bufferSize * sizeof(char));
    int i = 0, ret = 0, letterWidth, letterLineCount, letterCrit;
    char letterName,*lineList,*critList,*lineBuff,*critBuff;
    
    
    specFile = fopen(fileName, "r");
    if (specFile == NULL) {
        printf("Failed to open specification file\n");
        exit(0);
    }
    ret = fscanf(specFile, "%d %d\n", &letterCount, &letterHeight);
    if (ret != 2) {
        printf("Failed to read specification file\n");
        exit(1);
    } else {
        printf("Number of char: %d\n", letterCount);
        printf("Height of char: %d\n", letterHeight);
    }
    
    letters = malloc(letterCount*sizeof(struct VecLetter*));
    
    while ((getline(&buffer, &bufferSize, specFile) != -1) && (i < letterCount)) {
        lineList = (char*) malloc(256 * sizeof(char));
        critList = (char*) malloc(256 * sizeof(char));
        lineBuff = (char*) malloc(18 * sizeof(char));
        critBuff = (char*) malloc(18 * sizeof(char));
        ret = 0;
        ret = sscanf(buffer, "%c|%d|%d|%d|%[^|\n]|%[^\n]s", &letterName, &letterWidth, &letterLineCount, &letterCrit, lineList, critList);
        if (ret != 6) {
            printf("Error reading line %d\n",i+2);
        } else {
            letters[i] = vecLetterInit(letterName, 0, 0, letterHeight, letterWidth, letterLineCount, letterCrit);
            lineBuff = strtok(lineList," ");
            int j = 0;
            while (lineBuff != NULL) {
                double x1,y1,x2,y2;
                sscanf(lineBuff,"<(%lf,%lf),(%lf,%lf)>",&x1,&y1,&x2,&y2);
                letters[i]->lines[j] = lineInit(x1,y1,x2,y2);
                lineBuff = strtok(NULL," ");
                j++;
            }
            j = 0;
            critBuff = strtok(critList," ");
            while (critBuff != NULL) {
                double x,y;
                sscanf(critBuff,"(%lf,%lf)",&x,&y);
                letters[i]->critPoints[j] = pointInit(x,y);
                critBuff = strtok(NULL," ");
                j++;
            }
        }
        i++;
    }
    
    free(buffer);
    fclose(specFile);
}

int drawVector(char c, int x, int y, unsigned int border_color, unsigned int fill_color, float degree, int originX, int originY, float zoom) {
    // Find vector index
    int i = 0; int found = 0, idx = 0; 
    while (i < letterCount && found == 0) {
        if (letters[i]->name == c) {
            idx = i;
            found = 1;
        }
        i++;
    }

    if (found == 0) {
        return 0;
    }

    // Draw Vector border
    for (int j = 0; j < letters[idx]->numOfLines; j++) {

        //create rotated lines
        double x1,y1;
        x1 = letters[idx]->lines[j]->points[0]->x;
        y1 = letters[idx]->lines[j]->points[0]->y;
        //dilatate lines
        x1 = (x1 - originX)*zoom + originX;
        y1 = (y1 - originY)*zoom + originY;

        double t=(22*degree)/(180*7);
        double b1=((x1-originX)*cos(t))-((y1-originY)*sin(t)) + originX;
        double b2=((x1-originX)*sin(t))+((y1-originY)*cos(t)) +originY;

        double x2,y2;
        x2 = letters[idx]->lines[j]->points[1]->x;
        y2 = letters[idx]->lines[j]->points[1]->y;
        //dilatate lines
        x2 = (x2 - originX)*zoom + originX;
        y2 = (y2 - originY)*zoom + originY;
        double c1=((x2-originX)*cos(t))-((y2-originY)*sin(t)) + originX;
        double c2=((x2-originX)*sin(t))+((y2-originY)*cos(t)) +originY;

       struct Line * line = lineInit(b1,b2,c1,c2);
        drawLine_line(line, border_color, x, y);
       freeLine(line);
    }

    //fillLetter(letters[idx], fill_color, border_color, x, y);

    return letters[idx]->width;    
};

void fillLetter(struct VecLetter* vecletter, unsigned int color, unsigned int boundaryColor, int offsetX, int offsetY) {
    int isFilling = -1;
    int critExist = 0;
    if (vecletter->critPoints != NULL) {
        critExist = 1;
    }
    int critCounter = 0;
    
    // int isInside = 0;

    for (int j = offsetY; j <= vecletter->height + offsetY; j++) {
        isFilling = -1;
        for (int i= offsetX; i <= vecletter->width + offsetX; i++) {
            //crit point
            if (critCounter < vecletter->numOfCritPoints && critExist == 1 && i == vecletter->critPoints[critCounter]->x+offsetX && j == vecletter->critPoints[critCounter]->y+offsetY) {
                critCounter++;
                continue;
            }

            if (getPixelColor(i, j) == boundaryColor) {
                int initI = i;
                while(getPixelColor(i, j) == boundaryColor && i <= vecletter->width + offsetX) {
                    i++;
                }

                if (critCounter < vecletter->numOfCritPoints && critExist == 1 && i == vecletter->critPoints[critCounter]->x + offsetX && j == vecletter->critPoints[critCounter]->y + +offsetY) {
                    critCounter++;
                    // continue;
                } else {
                    isFilling *= -1;
                }
            }
            if (i <= vecletter->width + offsetX) {
                if (isFilling > 0) {
                    drawPixel(i, j, color);
                }
            }
            
        }
    }
}