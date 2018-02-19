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
#define WORLD_WIDTH 4000
#define WORLD_HEIGHT 2000
#define RUNNING 1

//Game World
unsigned int world[WORLD_WIDTH][WORLD_HEIGHT]; 
int viewport_x;
int viewport_y;
int viewport_width;
int viewport_height;

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

char *fbp = 0;
int centerX = 0;
int fullY = 0;

void render();
void clear(int color);

int isValidPoint(int x, int y);
void drawLineLow(double x0, double y0, double x1, double y1, unsigned int color);
void drawLineHigh(double x0, double y0, double x1, double y1, unsigned int color);
void drawVectorLine(VectorPoint* point1, VectorPoint* point2, unsigned int color, int offsetX, int offsetY);
int drawVectorPath(VectorPath* path, unsigned int color, int offsetX, int offsetY);

int rotatePath(VectorPath* path, float degree, int originX, int originY);
int dilatatePath(VectorPath* path, int originX, int originY, float zoom);
int translatePath(VectorPath* path, int dx, int dy);

void drawPixel(int x, int y, unsigned int color);
unsigned int rgbaToInt(int r, int g, int b, int a);
unsigned int getPixelColor(int x, int y);
VectorPoint** determineCriticalPoint(VectorPath* vecPath);
int isCritPointAlreadyExist(VectorPoint **arrCritPoint, int sizeOfArray, VectorPoint* vecPoint);
int isCriticalPoint(VectorPath* path, int _x, int _y);

int drawVector(char c, int x, int y, unsigned int border_color, unsigned int fill_color, float degree, int originX, int originY, float zoom);
void fillVector(VectorPath* path, unsigned int color, unsigned int boundaryColor);
int isCritPoint(int i, int j, unsigned int boundaryColor);

void swapPoint(VectorPoint* point1, VectorPoint* point2);
void bubbleSortPoint(VectorPoint** points, int size);

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


    // Initialize viewport
    viewport_width = vinfo.xres;
    viewport_height = vinfo.yres;

    // Tolong diubah saat sudah siap pakai ya
    // viewport_x = WORLD_WIDTH/2;
    // viewport_y = WORLD_HEIGHT/2;
    viewport_x = 40;
    viewport_y = 40;


    // Initialize configuration    
    int marginX = 100;
    int f;
    int posX = viewport_x;
    int posY = viewport_y + viewport_height/2;
    system("clear");
    float degree = 10;
    float wingDeg = 0;
    float zoom = 1;
    int first = 1;

    // Initialize vector objects
    VectorPath* rightTriangle = createVectorPathFromFile("badan_bawah.txt");
    if (rightTriangle == NULL) {
        printf("Failed to load triangle\n");
        return 0;
    }

    int count = 0;
    int dx = 10;


    // clear(rgbaToInt(0,0,0,0));
    dilatatePath(rightTriangle, rightTriangle->maxX / 2, rightTriangle->maxY / 2, 10);
    translatePath(rightTriangle, 250, 250);
    // drawVectorPath(rightTriangle, rgbaToInt(255,255,255,255), 0, 0);
    // for (int j = rightTriangle->minY; j <= rightTriangle->maxY; j++) {
    //     for (int i = rightTriangle->minY; i <= rightTriangle->maxY; i++) {
    //         if (i == rightTriangle->minY && j == rightTriangle->maxX) {
    //             drawPixel(i,j,rgbaToInt(0,255,0,0));
    //         }
    //         drawPixel(i,j,rgbaToInt(0,0,255,0));
    //     }
    // }
    // rotatePath(rightTriangle, 5, rightTriangle->maxX / 2, rightTriangle->maxY / 2);
    // fillVector(rightTriangle, rgbaToInt(255,0,0,0), rgbaToInt(255,255,255,255));
    // clear(rgbaToInt(0,0,0,0));
    // translatePath(rightTriangle, 100, 100);
    // drawVectorPath(rightTriangle, rgbaToInt(255,255,255,255), 0, 0);
    // fillVector(rightTriangle, rgbaToInt(255,0,0,0), rgbaToInt(255,255,255,255));
    // render();
    // printPath(rightTriangle);
    // printf("maxX : %d maxY: %d\n", rightTriangle->maxX, rightTriangle->maxY);
    // printf("minX : %d minY: %d\n", rightTriangle->minX, rightTriangle->minY);

    // VectorPoint** crit = determineCriticalPoint(rightTriangle);
    // for (int i = 0; i < rightTriangle->numOfPoints; i++) {
    //     if (crit[i] != NULL) {
    //         printf("%f, %f\n", crit[i]->x, crit[i]->y);
    //     }
    // }
    // bubbleSortPoint(crit, rightTriangle->numOfPoints);
    // for (int i = 0; i < rightTriangle->numOfPoints; i++) {
    //     if (crit[i] != NULL) {
    //         printf("%f, %f\n", crit[i]->x, crit[i]->y);
    //     }
    // }


    // Start animation and render
    while (RUNNING) {
        clear(rgbaToInt(0,0,0,0));
        // translatePath(rightTriangle, dx, 10);
        // rotatePath(rightTriangle, 10, rightTriangle->maxX / 2, rightTriangle->maxY / 2);
        dilatatePath(rightTriangle, rightTriangle->maxX / 2, rightTriangle->maxY / 2, 0.9);
        drawVectorPath(rightTriangle, rgbaToInt(255,255,255,255), 0, 0);
        fillVector(rightTriangle, rgbaToInt(255,0,0,0), rgbaToInt(255,255,255,255));
        render();
        usleep(300000);
    }


    return 0;
}

void render(){
    long int location;
    int color;

    for(int x = 0; x < viewport_width; x++){
        for(int y = 0; y < viewport_height; y++){
            int worldx = x + viewport_x;
            int worldy = y + viewport_y;
            if(isValidPoint(worldx,worldy)){
                color = world[worldx][worldy];
            }
            else{
                color = rgbaToInt(0,0,255,0);
            }
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
            *(fbp + location) = color;
            *(fbp + location + 1) = color >> 8;
            *(fbp + location + 2) = color >> 16;
            *(fbp + location + 3) = color >> 24;
        }
    }
}

void clear(int color){
    long int location;

    for(int x = viewport_x; x < viewport_width + viewport_x; x++){
        for(int y = viewport_y; y < viewport_height + viewport_y; y++){
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

VectorPoint** determineCriticalPoint(VectorPath* vecPath) {

    if (checkIfPathIsClosed(vecPath) == 1) {
        VectorPoint** critPoint = malloc((vecPath->numOfPoints) * sizeof(VectorPoint*));
        // VectorPoint* critPoint[vecPath->numOfPoints];

        int counter = 0;
        if (vecPath->firstPoint[0] != NULL)
        {
            VectorPoint **currentToCheck = vecPath->firstPoint;
            VectorPoint **nextToCheck = vecPath->firstPoint[0]->nextPoint;
            VectorPoint **prevToCheck = vecPath->firstPoint[0]->prevPoint;
            do
            {
                if ((nextToCheck[0]->y > currentToCheck[0]->y && prevToCheck[0]->y > currentToCheck[0]->y)
                    || (nextToCheck[0]->y < currentToCheck[0]->y && prevToCheck[0]->y < currentToCheck[0]->y))
                {
                    critPoint[counter] = currentToCheck[0];
                    counter += 1;
                }
                
                if (nextToCheck[0]->y == currentToCheck[0]->y) {
                    if (nextToCheck[0]->x > currentToCheck[0]->x) {
                        critPoint[counter] = nextToCheck[0];
                    } else {
                        critPoint[counter] = currentToCheck[0];
                    }
                    counter += 1;
                }

                prevToCheck = currentToCheck;
                currentToCheck = nextToCheck;
                if (currentToCheck[0] != NULL)
                {
                    nextToCheck = currentToCheck[0]->nextPoint;
                }
            } while (currentToCheck[0] != NULL && currentToCheck[0] != vecPath->firstPoint[0]);

            for (int i = counter; counter < vecPath->numOfPoints; counter++) {
                critPoint[counter] = NULL;
            }
        }
        else
        {
            printf("Path is empty\n");
        }

        return critPoint;

    } else {
        return NULL;
    }
}

int isCritPointAlreadyExist(VectorPoint **arrCritPoint, int sizeOfArray, VectorPoint *vecPoint) {
    int counter = 0;
    int found = 0;
    while (found == 0 && counter < sizeOfArray && arrCritPoint[counter] != NULL) {
        if (arrCritPoint[counter] == vecPoint) {
            found = 1;
        } else {
            counter += 1;
        }
    }

    return found;
}

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
}

void drawVectorLine(VectorPoint* point1, VectorPoint* point2, unsigned int color, int offsetX, int offsetY) {
    if (abs(point2->y - point1->y) < abs(point2->x - point1->x)) {
        if (point1->x > point2->x) {
            drawLineLow(point2->x + offsetX, point2->y + offsetY, point1->x + offsetX, point1->y + offsetY, color);
        } else {
            drawLineLow(point1->x + offsetX, point1->y + offsetY, point2->x + offsetX, point2->y + offsetY, color);
        }
    } else {
        if (point1->y > point2->y) {
            drawLineHigh(point2->x + offsetX, point2->y + offsetY, point1->x + offsetX, point1->y + offsetY, color);
        } else {
            drawLineHigh(point1->x + offsetX, point1->y + offsetY, point2->x + offsetX, point2->y + offsetY, color);
        }
    }
}

int drawVectorPath(VectorPath* path, unsigned int color, int offsetX, int offsetY) {
    if (path != NULL) {
        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            do {
                if (nextPoint[0] != NULL) {
                    drawVectorLine(currentPoint[0], currentPoint[0]->nextPoint[0], color, offsetX, offsetY);
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

    return 1;
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

void fillVector(VectorPath* path, unsigned int color, unsigned int boundaryColor) {
    int isFilling = -1;
    VectorPoint** critPoints = determineCriticalPoint(path);
    int count = 0;
    // for (int i = 0; i < path->numOfPoints; i++) {
    //     if (critPoints[i] != NULL) {
    //         // printf("%f, %f\n", critPoints[i]->x, critPoints[i]->y);
    //     }
    // }
    // printf("===============================\n");
    if (checkIfPathIsClosed(path)) {
        for (int j = path->minY; j <= path->maxY; j++) {
            isFilling = -1;
            for (int i = path->minX; i <= path->maxX; i++) {
                // printf("%d, %d\n", i, j);
                if (i == round(critPoints[count]->x) && j == round(critPoints[count]->y)) {
                    // printf("%f, %f\n", critPoints[count]->x, critPoints[count]->y);
                    count++;
                    continue;
                } else {
                    if (getPixelColor(i, j) == boundaryColor) {
                        while(getPixelColor(i, j) == boundaryColor && i <= path->maxX && i != round(critPoints[count]->x) && j != round(critPoints[count]->y)) {
                            i++;
                        }

                        if (i == round(critPoints[count]->x) && j == round(critPoints[count]->y)) {
                            // printf("%f, %f\n", critPoints[count]->x, critPoints[count]->y);
                            count++;
                        } else {
                            isFilling *= -1;
                        }
                    }
                    if (i <= path->maxX) {
                        if (isFilling > 0) {
                            drawPixel(i, j, color);
                        }
                    }
                }
            }
        }
    }
}

void swapPoint(VectorPoint* point1, VectorPoint* point2) {
    double _x = point1->x;
    double _y = point1->y;
    VectorPoint** next = point1->nextPoint;
    VectorPoint** prev = point1->prevPoint;

    point1->x = point2->x;
    point1->y = point2->y;
    point1->nextPoint = point2->nextPoint;
    point1->prevPoint = point2->prevPoint;

    point2->x = _x;
    point2->y = _y;
    point2->nextPoint = next;
    point2->prevPoint = prev;
}

void bubbleSortPoint(VectorPoint** points, int size) {
    int index = 0;
    int stillSorting = 1;
    int iteration = 0;

    // sorting y
    while (stillSorting > 0 && index < size - 1) {
        index = 0;
        stillSorting = 0;

        iteration++;
        // printf("iteration %d\n", iteration);
        // printf("%f ? %f\n", points[index]->y, points[index + 1]->y);
        if (points[index] != NULL && points[index + 1] != NULL && points[index]->y > points[index + 1]->y) {
            // printf("swapped\n");
            swapPoint(points[index], points[index + 1]);
            stillSorting++;
        }

        index++;
    }

    index = 0;
    stillSorting = 1;
    int lastIndex = 0;
    // sorting x
    while (lastIndex < size) {
        // printf("%d\n", lastIndex);
        index = lastIndex;
        if (points[index]->y == points[index + 1]->y) {
            while(index < size - 1 && points[index]->y == points[index + 1]->y && stillSorting > 0) {
                // printf("index %d\n", index);
                stillSorting = 0;
                if (points[index] != NULL && points[index + 1] != NULL && points[index] != NULL && points[index + 1] != NULL && points[index]->x > points[index + 1]->x) {
                    // printf("swapped\n");
                    swapPoint(points[index], points[index + 1]);
                    stillSorting++;
                }
                index++;
            }
        } else {
            index++;
        }
        lastIndex = index;
    }
}