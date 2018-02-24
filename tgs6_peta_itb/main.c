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
#include "headers/VectorPath.h"
#include <math.h>


#define SCALE 1
#define WORLD_WIDTH 2000
#define WORLD_HEIGHT 2000
#define VIEWPORT_SPEED 5
#define RUNNING 1

//Game World
unsigned int world[WORLD_WIDTH][WORLD_HEIGHT]; 
int viewport_x;
int viewport_y;
int viewport_width = 640;
int viewport_height = 480;
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

char *fbp = 0;
int centerX = 0;
int fullY = 0;
int frameColor;
int critColor;
void render();
void clearViewPort(int color);
void clearScreen();
int isValidPoint(int x, int y);
void drawLineLow(double x0, double y0, double x1, double y1, unsigned int color);
void drawLineHigh(double x0, double y0, double x1, double y1, unsigned int color);
void drawVectorLine(VectorPoint* point1, VectorPoint* point2, unsigned int color, int offsetX, int offsetY);
int drawVectorPath(VectorPath* path, unsigned int boundaryColor, unsigned int color, int offsetX, int offsetY);
void drawCritPoint(VectorPath* path, int offsetX, int offsetY, unsigned int boundaryColor);
int rotatePath(VectorPath* path, float degree, int originX, int originY);
int dilatatePath(VectorPath* path, int originX, int originY, float zoom);
int translatePath(VectorPath* path, int dx, int dy);
void drawPixel(int x, int y, unsigned int color);
unsigned int rgbaToInt(int r, int g, int b, int a);
unsigned int getPixelColor(int x, int y);
void determineCriticalPoint(VectorPath* vecPath);
void fillVector(VectorPath* path, unsigned int fillColor, unsigned int boundaryColor, int offsetX, int offsetY);
int isCritPoint(int i, int j, unsigned int boundaryColor);

// int isCritPointAlreadyExist(VectorPoint **arrCritPoint, int sizeOfArray, VectorPoint* vecPoint);
// int isCriticalPoint(VectorPath* path, int _x, int _y);
// void swapPoint(VectorPoint* point1, VectorPoint* point2);
// void bubbleSortPoint(VectorPoint** points, int size);

int main() {
    // tak perlu disentuh lagi {
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
    // }
    if(viewport_width > vinfo.xres || viewport_height > vinfo.yres){
        printf("Ukuran viewport salah\n");

        return 0;
    }

    critColor = rgbaToInt(250,250,250,0);
    frameColor = rgbaToInt(247,247,247,0);
    viewport_x = 250;
    viewport_y = 250;

    // Initialize vector objects
    VectorPath* badan_bawah = createVectorPathFromFile("badan_bawah.txt");
    if (badan_bawah == NULL) {
        printf("Failed to load badan bawah\n");
        return 0;
    }
    VectorPath* sayap_utama = createVectorPathFromFile("sayap.txt");
    if (sayap_utama == NULL) {
        printf("Failed to load sayap utama\n");
        return 0;
    }
    VectorPath* sayap_belakang = createVectorPathFromFile("sayap_belakang.txt");
    if (sayap_belakang == NULL) {
        printf("Failed to load sayap belakang\n");
        return 0;
    }
    VectorPath* baling_baling = createVectorPathFromFile("baling2.txt");
    if (baling_baling == NULL) {
        printf("Failed to load baling-baling\n");
        return 0;
    }

    int count = 0;
    int dx = 10;

    dilatatePath(badan_bawah, 50, 50, 5);
    dilatatePath(sayap_belakang, 50, 50, 5);
    dilatatePath(sayap_utama, 50, 55, 5);
    dilatatePath(baling_baling, 50, 60, 5);

    clearScreen();
    rotatePath(baling_baling, 10,  50, 60);

    // Start animation and render
   while (RUNNING) {
       // clearViewPort(rgbaToInt(10,10,10,0));

        clearViewPort(rgbaToInt(135,206,250,0));
        // translatePath(rightTriangle, dx, 10);
        rotatePath(baling_baling, 10,  50, 60);

        drawVectorPath(sayap_belakang, rgbaToInt(0,0,0,0),rgbaToInt(107,107,107,0), 500, 500);
        drawVectorPath(badan_bawah, rgbaToInt(2,2,2,0),rgbaToInt(48,60,165,0), 500, 500);

        drawVectorPath(sayap_utama, rgbaToInt(1,1,1,0),rgbaToInt(196,0,0,0), 500, 500);

        drawVectorPath(baling_baling, rgbaToInt(3,3,3,0),rgbaToInt(102,66,0,0), 500, 500);

        render();
        char c;
        scanf("%c", &c);
        if(c == 'w' || c == 'W'){
            viewport_y -= VIEWPORT_SPEED;
        } else if(c == 'a' || c == 'A'){
            viewport_x -= VIEWPORT_SPEED;
        } else if(c == 's' || c == 'S'){
            viewport_y += VIEWPORT_SPEED;
        } else if(c == 'd' || c == 'D'){
            viewport_x += VIEWPORT_SPEED;
        } 
        if(viewport_x < 0)
            viewport_x = 0;
        if(viewport_y < 0)
            viewport_y = 0;
        if(viewport_x > WORLD_WIDTH - viewport_width)
            viewport_x = WORLD_WIDTH - viewport_width;
        if(viewport_y > WORLD_HEIGHT - viewport_height)
            viewport_y = WORLD_HEIGHT - viewport_height;
        usleep(3000);
  }


    return 0;
}
void clearScreen(){
    long int location;
    int color = rgbaToInt(0,0,0,0);
    for(int x = 0; x < vinfo.xres; x++){
        for(int y = 0; y < vinfo.yres; y++){
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = color;
            *(fbp + location + 1) = color >> 8;
            *(fbp + location + 2) = color >> 16;
            *(fbp + location + 3) = color >> 24; 
        }
    }
}

void render(){
    long int location;
    int color;
    int offsetX = (vinfo.xres - viewport_width)/2;
    int offsetY = (vinfo.yres - viewport_height)/2;


    for(int x = 0; x < viewport_width; x++){
        for(int y = 0; y < viewport_height; y++){
            int worldx = x + viewport_x;
            int worldy = y + viewport_y;
            // if(x == 0 || y == 0 || x == viewport_width -1 || y == viewport_height -1){
            //     color = frameColor;
            // } else
            if(isValidPoint(worldx,worldy)){
                color = world[worldx][worldy];
            }
            else{
                color = rgbaToInt(0,0,0,0);
            }
            location = (x+offsetX+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+offsetY+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = color;
            *(fbp + location + 1) = color >> 8;
            *(fbp + location + 2) = color >> 16;
            *(fbp + location + 3) = color >> 24;
        }
    }
}

void clearViewPort(int color){
    long int location;

    for(int x = 0; x < viewport_width + viewport_x; x++){
        for(int y = 0; y < viewport_height + viewport_y; y++){
            world[x][y] = color;
        }
    }
}
unsigned int rgbaToInt(int r, int g, int b, int a) {
    return a << 24 | r << 16 | g << 8 | b;
}

unsigned int getPixelColor(int x, int y) {
    if(!isValidPoint(x,y)){
        return 0;
    }
    x = x * SCALE;
    y = y * SCALE;

   return world[x][y];
}

void determineCriticalPoint(VectorPath* vecPath) {

    if (checkIfPathIsClosed(vecPath) == 1) {
        if (vecPath->firstPoint[0] != NULL)
        {
            VectorPoint **currentToCheck = vecPath->firstPoint;
            VectorPoint **nextToCheck = vecPath->firstPoint[0]->nextPoint;
            VectorPoint **prevToCheck = vecPath->firstPoint[0]->prevPoint;

            int nextX, prevX, currentX, nextY, currentY, prevY;
            do
            {    currentToCheck[0]->isCrit = 0;
                nextX = round(nextToCheck[0]->x);
                currentX = round(currentToCheck[0]->x);
                nextY = round(nextToCheck[0]->y);
                currentY = round(currentToCheck[0]->y);
                prevY = round(prevToCheck[0]->y);

                if ((nextY > currentY && prevY > currentY)
                    || (nextY < currentY && prevY < currentY))
                {
                    currentToCheck[0]->isCrit = 1;
                }
                
                if (nextY == currentY) {
                    int nextnextY = round(nextToCheck[0]->nextPoint[0]->y);
                    if ((nextnextY > currentY && prevY > currentY)
                        || (nextnextY < currentY && prevY < currentY))
                        {
                            if (nextX > currentX) {
                                nextToCheck[0]->isCrit = 1;

                            } else {
                                currentToCheck[0]->isCrit = 1;

                            }
                        }
                }

                prevToCheck = currentToCheck;
                currentToCheck = nextToCheck;
                if (currentToCheck[0] != NULL)
                {
                    nextToCheck = currentToCheck[0]->nextPoint;
                }
            } while (currentToCheck[0] != NULL && currentToCheck[0] != vecPath->firstPoint[0]);

        }
        else
        {
            printf("Path is empty\n");
        }

        return;

    } else {
        return;
    }
}

// int isCritPointAlreadyExist(VectorPoint **arrCritPoint, int sizeOfArray, VectorPoint *vecPoint) {
//     int counter = 0;
//     int found = 0;
//     while (found == 0 && counter < sizeOfArray && arrCritPoint[counter] != NULL) {
//         if (arrCritPoint[counter] == vecPoint) {
//             found = 1;
//         } else {
//             counter += 1;
//         }
//     }

//     return found;
// }

void drawPixel(int x, int y, unsigned int color) {
    int i = 0, j = 0;
    x = x*SCALE; y = y*SCALE;
    for (i = 0; i < SCALE; i++)
        for (j = 0; j < SCALE; j++) {
            world[x+i][y+j] = color;
        }
}

int isValidPoint(int x, int y) {
    if (x >= 0 && x < WORLD_WIDTH/SCALE && y >=0 && y < WORLD_HEIGHT/SCALE)
        return 1;

    return 0;
}

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

    
    if (isValidPoint(round(x0), round(y0)))
        drawPixel(x0, y0, color);
    if (isValidPoint(round(x1), round(y1)))
        drawPixel(x1, y1, color);
}

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
        if (isValidPoint(round(x0), round(y0)))
        drawPixel(x0, y0, color);
    if (isValidPoint(round(x1), round(y1)))
        drawPixel(x1, y1,color);
}

void drawVectorLine(VectorPoint* point1, VectorPoint* point2, unsigned int color, int offsetX, int offsetY) {
    double x1,y1,x2,y2;
    x1 = round(point1->x);
    y1 = round(point1->y);
    x2 = round(point2->x);
    y2 = round(point2->y);
    if (abs(y2 - y1) < abs(x2 - x1)) {
        if (x1 > x2) {
            drawLineLow(x2 + offsetX, y2 + offsetY, x1 + offsetX, y1 + offsetY, color);
        } else {
            drawLineLow(x1 + offsetX, y1 + offsetY, x2 + offsetX, y2 + offsetY, color);
        }
    } else {
        if (y1 > y2) {
            drawLineHigh(x2 + offsetX, y2 + offsetY, x1 + offsetX, y1 + offsetY, color);
        } else {
            drawLineHigh(x1 + offsetX, y1 + offsetY, x2 + offsetX, y2 + offsetY, color);
        }
    }
}

int drawVectorPath(VectorPath* path, unsigned int boundaryColor, unsigned int fillColor, int offsetX, int offsetY) {
    if (path != NULL) {

        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            do {
                if (nextPoint[0] != NULL) {
                    drawVectorLine(currentPoint[0], currentPoint[0]->nextPoint[0], boundaryColor, offsetX, offsetY);
                }


                currentPoint = nextPoint;

                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            return 0;
        }
    } else {
        return 0;
    }

   fillVector(path, fillColor, boundaryColor, offsetX, offsetY);

   
    return 1;
}

VectorPath** clipVector(VectorPath* pathToClip, int viewport_x, int viewporty_y, int viewport_width, int viewport_height) {
    if (path != NULL) {
        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            double t=(22*degree)/(180*7);

            do {
                double x1 = currentPoint[0]->x;
                double y1 = currentPoint[0]->y;

                currentPoint[0]->x = ((x1-originX)*cos(t))-((y1-originY)*sin(t)) + originX;
                currentPoint[0]->y = ((x1-originX)*sin(t))+((y1-originY)*cos(t)) +originY;

                currentPoint = nextPoint;
                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            return 0;
        }
    } else {
        return NULL;
    }
}

int rotatePath(VectorPath* path, float degree, int originX, int originY) {
    if (path != NULL) {
        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            double t=(22*degree)/(180*7);

            do {
                double x1 = currentPoint[0]->x;
                double y1 = currentPoint[0]->y;

                currentPoint[0]->x = ((x1-originX)*cos(t))-((y1-originY)*sin(t)) + originX;
                currentPoint[0]->y = ((x1-originX)*sin(t))+((y1-originY)*cos(t)) +originY;

                currentPoint = nextPoint;
                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            return 0;
        }
    } else {
        return 0;
    }

   checkForMinMaxUpdate(path);
    return 1;
}

int dilatatePath(VectorPath* path, int originX, int originY, float zoom) {
    if (path != NULL) {
        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            do {
                double x1 = currentPoint[0]->x;
                double y1 = currentPoint[0]->y;
                currentPoint[0]->x = (x1 - originX) * zoom + originX;
                currentPoint[0]->y = (y1 - originY) * zoom + originY;


                currentPoint = nextPoint;
                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            return 0;
        }
    } else {
        return 0;
    }

    checkForMinMaxUpdate(path);
    return 1;
}

int translatePath(VectorPath* path, int dx, int dy) {
    if (path != NULL) {
        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            do {
                double x1 = currentPoint[0]->x;
                double y1 = currentPoint[0]->y;
                currentPoint[0]->x = (x1 + dx);
                currentPoint[0]->y = (y1 + dy);

                currentPoint = nextPoint;
                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            return 0;
        }
    } else {
        return 0;
    }

    checkForMinMaxUpdate(path);
    return 1;
}

void drawCritPoint(VectorPath* path, int offsetX, int offsetY, unsigned int boundaryColor){
    if (path != NULL) {
        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            do {

                if(currentPoint[0]->isCrit){
                    int i = round(currentPoint[0]->x) + offsetX;
                    int j = round(currentPoint[0]->y)+ offsetY;
                    while(isValidPoint(i,j) && getPixelColor(i,j) == boundaryColor){
                        i++;
                    }    
                    drawPixel(i-1, j, critColor);

                }

                currentPoint = nextPoint;
                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            return;
        }
    } else {
        return;
    } 
}

void fillVector(VectorPath* path, unsigned int fillColor, unsigned int boundaryColor, int offsetX, int offsetY) {
    int isFilling = -1;

    int count = 0;
    // for (int i = 0; i < path->numOfPoints; i++) {
    //     if (critPoints[i] != NULL) {
    //         // printf("%f, %f\n", critPoints[i]->x, critPoints[i]->y);
    //     }
    // }
    // // printf("===============================\n");
    determineCriticalPoint(path);
    drawCritPoint(path, offsetX, offsetY, boundaryColor);

    if (checkIfPathIsClosed(path)) {
        for (int j = path->minY-1 + offsetY; j <= path->maxY + offsetY; j++) {
            isFilling = -1;
            for (int i = path->minX + offsetX -1; i <= path->maxX + offsetX; i++) {
                // printf("%d, %d\n", i, j);
                if (getPixelColor(i, j) == critColor) {
                    drawPixel(i,j, boundaryColor);
                    continue;
                } else {
                    if (getPixelColor(i, j) == boundaryColor || getPixelColor(i, j) == critColor) {
                        while(getPixelColor(i, j) == boundaryColor && i <= path->maxX + offsetX && getPixelColor(i, j) != critColor) {
                            i++;
                        }

                        if (getPixelColor(i, j) == critColor) {
                            drawPixel(i,j, boundaryColor);

                        } else {
                            isFilling *= -1;
                        }
                    }
                    if (i <= path->maxX+ offsetX) {
                        if (isFilling > 0) {
                            drawPixel(i, j, fillColor);
                        }
                    }
                }
            }
        }
    }

}

// void swapPoint(VectorPoint* point1, VectorPoint* point2) {
//     double _x = point1->x;
//     double _y = point1->y;
//     VectorPoint** next = point1->nextPoint;
//     VectorPoint** prev = point1->prevPoint;

//     point1->x = point2->x;
//     point1->y = point2->y;
//     point1->nextPoint = point2->nextPoint;
//     point1->prevPoint = point2->prevPoint;

//     point2->x = _x;
//     point2->y = _y;
//     point2->nextPoint = next;
//     point2->prevPoint = prev;
// }

// void bubbleSortPoint(VectorPoint** points, int size) {
//     int index = 0;
//     int stillSorting = 1;
//     int iteration = 0;

//     // sorting y
//     while (stillSorting > 0 && index < size - 1) {
//         index = 0;
//         stillSorting = 0;

//         iteration++;
//         // printf("iteration %d\n", iteration);
//         // printf("%f ? %f\n", points[index]->y, points[index + 1]->y);
//         if (points[index] != NULL && points[index + 1] != NULL && points[index]->y > points[index + 1]->y) {
//             // printf("swapped\n");
//             swapPoint(points[index], points[index + 1]);
//             stillSorting++;
//         }

//         index++;
//     }

//     index = 0;
//     stillSorting = 1;
//     int lastIndex = 0;
//     // sorting x
//     while (lastIndex < size) {
//         // printf("%d\n", lastIndex);
//         index = lastIndex;
//         if (points[index]->y == points[index + 1]->y) {
//             while(index < size - 1 && points[index]->y == points[index + 1]->y && stillSorting > 0) {
//                 // printf("index %d\n", index);
//                 stillSorting = 0;
//                 if (points[index] != NULL && points[index + 1] != NULL && points[index] != NULL && points[index + 1] != NULL && points[index]->x > points[index + 1]->x) {
//                     // printf("swapped\n");
//                     swapPoint(points[index], points[index + 1]);
//                     stillSorting++;
//                 }
//                 index++;
//             }
//         } else {
//             index++;
//         }
//         lastIndex = index;
//     }

// }