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

#define SCALE 2

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

char *fbp = 0;
int centerX = 0;
int fullY = 0;

int letterCount, letterHeight;
struct VecLetter** letters;

unsigned int rgbaToInt(int r, int g, int b, int a);
void drawPixel(int x, int y, unsigned int color);
void drawPixelWithScale(int x, int y, unsigned int color, int scale);

int isValidPoint(int x, int y);
int isValidPointScale(int x, int y, int scale);
unsigned int getPixelColor(int x, int y);
void drawLineLow(double x0, double y0, double x1, double y1, int color);
void drawLineLowWithScale(double x0, double y0, double x1, double y1, int color, int scale);
void drawLineHigh(double x0, double y0, double x1, double y1, int color);
void drawLineHighWithScale(double x0, double y0, double x1, double y1, int color, int scale);
void drawLine(double x0, double y0, double x1, double y2, int color);
void drawLineWithScale(double x0, double y0, double x1, double y2, int color, int Scale);
void drawLaser(int x0, int y0, int dx, int dy, int scale, int* pt);
void drawLine_line(struct Line* line, int color, int pengali);
void loadLetters(char* fileName);
void fillLetter(struct VecLetter *vecletter, unsigned int color, unsigned int boundaryColor, int pengali);

    int main()
{

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
    system("clear");

    // Gambar dengan point (hijau)
    /*
    struct Point* point1 = pointInit(500/3,250);
    struct Point* point2 = pointInit(500*2/3,250);
    struct Point* point3 = pointInit(500*2/6,100);

    drawLine(point1->x, point1->y, point2->x, point2->y, rgbaToInt(0, 255,0, 0));    
    drawLine(point2->x, point2->y, point3->x, point3->y, rgbaToInt(0, 255,0, 0));
    drawLine(point1->x, point1->y, point3->x, point3->y, rgbaToInt(0, 255,0, 0));    

    freePoint(point1);
    freePoint(point2);
    freePoint(point3);



    // Gambar dengan line (biru)
    struct Line* line1 = lineInit(500/3,250, 500/6, 250);
    struct Line* line2 = lineInit(500/6,250, 500/6, 0);
    struct Line* line3 = lineInit(500/6,0, 500/3, 250);

    drawLine_line(line1, rgbaToInt(0,0,255,0));
    drawLine_line(line2, rgbaToInt(0,0,255,0));
    drawLine_line(line3, rgbaToInt(0,0,255,0));

    freeLine(line1);
    freeLine(line2);
    freeLine(line3);
    */


    // Gambar dengan VecLetter (merah); Masih hardcode -> Buat fungsi stroke(VecLetter, lineColor) mungkin?;
    // freeVecLetter sudah mem-free lines dan points di dalamnya, walau lines dan points di-malloc dari luar.

    // double posX = 30;
    // double posY = 15;
    // double width = 210;
    // double height = 280;

    // struct Line** lines = malloc(12 * sizeof(struct Line*));

    // // frame luar
    // lines[0] = lineInit(posX, posY, posX + width, posY);
    // lines[1] = lineInit(posX + width, posY, posX + width, posY + height);
    // lines[2] = lineInit(posX + width, posY + height, posX + width - 50, posY + height);
    // lines[3] = lineInit(posX + width - 50, posY + height, posX + width - 50, posY + height - 100);
    // lines[4] = lineInit(posX + width - 50, posY + height -100, posX + width - 50 - 110, posY + height - 100);
    // lines[5] = lineInit(posX + width - 50 - 110, posY + height - 100, posX + width - 50 - 110, posY + height);
    // lines[6] = lineInit(posX + width - 50 - 110, posY + height, posX, posY + height);
    // lines[7] = lineInit(posX, posY + height, posX, posY);

    // // lubang di dalam
    // lines[8] = lineInit(posX + 50, posY + 50, posX + 50 + 110, posY + 50);
    // lines[9] = lineInit(posX + 50 + 110, posY + 50, posX + 50 + 110, posY + 50 + 65);
    // lines[10] = lineInit(posX + 50 + 110, posY + 50 + 65, posX + 50, posY + 50 + 65);
    // lines[11] = lineInit(posX + 50, posY + 50 + 65, posX + 50, posY + 50);


    
    loadLetters("spec.txt");
    
    char input[100];
    // printf("cp A = %f, %f \n", letters[0]->critPoints[1]->x, letters[0]->critPoints[1]->y);
    printf("%s: ", "Masukkan input");
    scanf("%99[0-9a-zA-Z ]", input);

    for(int i = 0; input[i]; i++) {
        input[i] = tolower(input[i]);
    }

    for (int i=0; i<vinfo.yres/17; i++) {
      printf("\n");
    }
    printf("\n");
    int f;

    int pengali = -20;
    system("clear");
    
    for (int i=0;i<strlen(input);i++) {
      // for (int i=0; i<10; i++) {
          // printf("\n");
      // }
      // printf("Number of Lines: %d\n",letters[i]->numOfLines);
      pengali += 25;
      if (input[i] == ' ') continue;
      if ((input[i] - 97) > 25 || (input[i] - 97 < 0)) {
          pengali -= 25;
          continue;
      }
      for (int j=0;j<letters[input[i]-97]->numOfLines;j++) {
            drawLine_line(letters[input[i]-97]->lines[j], rgbaToInt(255,0,0,0), pengali);
      }

      unsigned int fillColor = rgbaToInt(255, 0, 0, 0);
      unsigned int boundaryColor = rgbaToInt(255, 0, 0, 0);
    //   unsigned int fillColor = 0x00ffffff;
    //   unsigned int boundaryColor = 0x00ff0000;
        
      fillLetter(letters[input[i] - 97], fillColor, boundaryColor, pengali);

      usleep(50000);
      // printf("Number of Critical Points: %d\n",letters[i]->numOfCritPoints);
      // for (int k=0;k<letters[i]->numOfCritPoints;k++) {
      //     printf("x: %lf,y: %lf\n",letters[i]->critPoints[k]->x,letters[i]->critPoints[k]->y);
      // }
    }
    
    for (int i=0; i<vinfo.yres/17; i++) {
      printf("\n");
    }
    
    // printf("%d\n", rgbaToInt(255, 255, 255, 0));
    return 0;
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
    unsigned int red = *(fbp + location + 2) ^ 0xffffff00;
    unsigned int alpha = *(fbp + location + 3);

    
    return (alpha << 24 | red << 16 | green << 8 | blue);
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

void drawPixelWithScale(int x, int y, unsigned int color, int scale) {
    long int location;
    int i = 0, j = 0;
    x = x*scale; y = y*scale;
    for (i = 0; i < scale; i++)
        for (j = 0; j < scale; j++) {
            location = (x+i+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+j+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = color;
            *(fbp + location + 1) = color >> 8;
            *(fbp + location + 2) = color >> 16;
            *(fbp + location + 3) = color >> 24;
        }
}

int isValidPoint(int x, int y) {
    if (x >= 0 && x < vinfo.xres/SCALE && y >=0 && y < vinfo.yres/SCALE)
        return 1;

    return 0;
}

int isValidPointScale(int x, int y, int scale) {
    if (x >= 0 && x < vinfo.xres/scale && y >=0 && y < vinfo.yres/scale)
        return 1;

    return 0;
}

void drawLineLow(double x0, double y0, double x1, double y1, int color) {
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

    for (double x = x0; x < x1; x++) {
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

void drawLineLowWithScale(double x0, double y0, double x1, double y1, int color, int scale) {
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

    for (double x = x0; x < x1; x++) {
        if (isValidPointScale(x, y, scale) == 0)
            return;
        drawPixelWithScale(x, y, color, scale);
        if (D > 0) {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

void drawLineHigh(double x0, double y0, double x1, double y1, int color) {
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

    for (double y = y0; y < y1; y++) {
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

void drawLineHighWithScale(double x0, double y0, double x1, double y1, int color, int scale) {
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

    for (double y = y0; y < y1; y++) {
        if (isValidPointScale(x, y, scale) == 0)
            return;
        drawPixelWithScale(x, y, color, scale);
        if (D > 0) {
            x += xi;
            D -= 2 * dy;
        }

        D += 2 * dx;
    }
}

void drawLine(double x0, double y0, double x1, double y1, int color) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            drawLineLow(x1, y1, x0, y0, color);
        } else {
            drawLineLow(x0, y0, x1, y1, color);
        }
    } else {
        if (y0 > y1) {
            drawLineHigh(x1, y1, x0, y0, color);
        } else {
            drawLineHigh(x0, y0, x1, y1, color);
        }
    }
}

void drawLine_line(struct Line* line, int color, int pengali) {
    if (abs(line->points[1]->y - line->points[0]->y) < abs(line->points[1]->x - line->points[0]->x)) {
        if (line->points[0]->x > line->points[1]->x) {
            drawLineLow(line->points[1]->x + pengali, line->points[1]->y, line->points[0]->x + pengali, line->points[0]->y, color);
        } else {
            drawLineLow(line->points[0]->x + pengali, line->points[0]->y, line->points[1]->x + pengali, line->points[1]->y, color);
        }
    } else {
        if (line->points[0]->y > line->points[1]->y) {
            drawLineHigh(line->points[1]->x + pengali, line->points[1]->y, line->points[0]->x + pengali, line->points[0]->y, color);
        } else {
            drawLineHigh(line->points[0]->x + pengali, line->points[0]->y, line->points[1]->x + pengali, line->points[1]->y, color);
        }
    }
}

void drawLineWithScale(double x0, double y0, double x1, double y1, int color, int scale) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            drawLineLowWithScale(x1, y1, x0, y0, color, scale);
        } else {
            drawLineLowWithScale(x0, y0, x1, y1, color, scale);
        }
    } else {
        if (y0 > y1) {
            drawLineHighWithScale(x1, y1, x0, y0, color, scale);
        } else {
            drawLineHighWithScale(x0, y0, x1, y1, color, scale);
        }
    }
}

void drawLaser(int x0, int y0, int dx, int dy, int scale, int* pt) {
    int t = *pt;
    if (isValidPointScale(x0 + dx * t, y0 + dy * t, scale) == 0) {
        t = 1; *pt = 1;
    }

    drawLineWithScale(x0, y0, x0 + dx * t, y0 + dy * t, rgbaToInt(0,0,0,0), scale);
    x0 = x0 + dx * t;
    y0 = y0 + dy * t;
    drawLineWithScale(x0, y0, x0 + dx * t, y0 + dy * t, rgbaToInt(0,255,0,0), scale);
}

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
        }
        else {
            letters[i] = vecLetterInit(0,0,letterHeight, letterWidth, letterLineCount, letterCrit);
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

void fillLetter(struct VecLetter* vecletter, unsigned int color, unsigned int boundaryColor, int pengali)
{
    int isFilling = -1;
    int critExist = 0;
    if (vecletter->critPoints != NULL) {
        critExist = 1;
    }
    int critCounter = 0;
    
    // int isInside = 0;

    for (int j = 0; j <= vecletter->height; j++) {
        isFilling = -1;
        for (int i= pengali; i <= vecletter->width + pengali; i++) {
        {
            
            //crit point
            if (critCounter < vecletter->numOfCritPoints && critExist == 1 && i == vecletter->critPoints[critCounter]->x+pengali && j == vecletter->critPoints[critCounter]->y)
            {
                critCounter++;
                continue;
            }

            if (getPixelColor(i, j) == boundaryColor)
            {
                while(getPixelColor(i, j) == boundaryColor && i <= vecletter->width + pengali) {
                    i++;
                }

                if (critCounter < vecletter->numOfCritPoints && critExist == 1 && i == vecletter->critPoints[critCounter]->x + pengali && j == vecletter->critPoints[critCounter]->y)
                {
                    critCounter++;
                    // continue;
                } else {
                    isFilling *= -1;
                }
            }
            if (i <= vecletter->width + pengali) {
                if (isFilling > 0)
                {
                    drawPixel(i, j, color);
                }
            }
            
        }
    }
}}