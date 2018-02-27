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
#define VIEWPORT_SPEED 20
#define RUNNING 1

#define BOTTOM 2
#define TOP 1
#define LEFT 8
#define RIGHT 4
#define TOP_LEFT 9 
#define TOP_RIGHT 5
#define BOTTOM_LEFT 10 
#define BOTTOM_RIGHT 6
#define MIDDLE 0

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
int getPointCode(int x, int y, double offsetX, double offsetY);
VectorPath * vectorClipping (VectorPath * path, int offsetX, int offsetY);
int drawVectorPathClipping(VectorPath* path, unsigned int boundaryColor, unsigned int fillColor, int offsetX, int offsetY);
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

// new in tugas_6
void drawCircle(int x0, int y0, int radius, unsigned int boundaryColor, unsigned int fillColor);
void fillCircle(unsigned int boundaryColor, unsigned int fillColor);

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
    viewport_x = 200;
    viewport_y = 500;

    int numOfGedung = 25;
    VectorPath** gedung = createVectorPathFromSVG("path1.txt", numOfGedung);
    VectorPath** jalan = createVectorPathFromSVG("path2.txt", 1);
    // clearScreen();
    // drawCircle(viewport_x + 100, viewport_y + 250, 150, rgbaToInt(255,0,0,0), rgbaToInt(0,0,255,0));
    // drawCircle(viewport_x + 350, viewport_y + 250, 100, rgbaToInt(9,255,0,0), rgbaToInt(0,255,0,0));
    // drawCircle(viewport_x + 200, viewport_y + 200, 150, rgbaToInt(0,0,255,0), rgbaToInt(255,0,0,0));
    // drawVectorPath(gedung[0], rgbaToInt(255,255,255,0), rgbaToInt(0,0,255,0), 0, 0);
    // drawVectorPath(gedung[1], rgbaToInt(255,225,255,0), rgbaToInt(255,0,0,0), 0, 0);
    // drawVectorPath(gedung[2], rgbaToInt(255,245,255,0), rgbaToInt(0,255,0,0), 0, 0);
    // render();
    // for (int i = 0; i < numOfGedung; i++) {
    //     printPath(gedung[i]);
    //     // freeVectorPath(gedung[i]);
    // }



   // Start animation and render
   while (RUNNING) {
        clearScreen();
        for (int i = 0; i < numOfGedung; i++) {
            drawVectorPath(gedung[i], rgbaToInt(255,255,200 + i,0), rgbaToInt(0,0,150 + i,0), 0, 0);
            // drawVectorPathClipping(gedung[i], rgbaToInt(255,255,200 + i,0), rgbaToInt(0,0,150 + i,0), 0, 0);
            // freeVectorPath(gedung[i]);
        }
        drawVectorPath(jalan[0], rgbaToInt(255,255,199,0), rgbaToInt(200,200,200,0), 0, 0);

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
  
    }

    free(gedung);

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

void drawPixel(int x, int y, unsigned int color) {
    int i = 0, j = 0;
    x = x*SCALE; y = y*SCALE;
    for (i = 0; i < SCALE; i++) {
        for (j = 0; j < SCALE; j++) {
            world[x+i][y+j] = color;
        }
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

VectorPath * vectorClipping (VectorPath * path, int offsetX, int offsetY){
    VectorPath * clipped = NULL;
    if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {


        VectorPoint** currentPoint = path->firstPoint;
        VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;
        int isFirst = 1;
        int pointke=1;
        do {

            //Clipping code
            int code_point1 = getPointCode(currentPoint[0]->x,currentPoint[0]->y, offsetX, offsetY);
            int code_point2 = getPointCode(nextPoint[0]->x,nextPoint[0]->y, offsetX, offsetY);
            pointke++;

            int xmax = viewport_x + viewport_width;
            int ymax = viewport_y + viewport_height;
            int ymin = viewport_y;
            int xmin = viewport_x;
            double x1 = currentPoint[0]->x + offsetX;
            double y1 = currentPoint[0]->y + offsetY;
            double x2 = nextPoint[0]->x + offsetY;
            double y2 = nextPoint[0]->y +offsetX;


            if(code_point1 == 0 && code_point2 == 0){
                
                if(isFirst == 1){
                    clipped = createVectorPath(createVectorPoint(x2-offsetX, y2- offsetY));
                    isFirst = 0;

                } else {
                    appendToPath(clipped, createVectorPoint(x2-offsetX, y2 - offsetY));
                }

            } else if(code_point1 != 0 && code_point2 != 0){
                //donothing

            } else{

                double x, y;

                int code_point_ex = code_point1 ? code_point1 : code_point2;
                if (code_point_ex & TOP)
                {
                    x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
                    y = ymin;
                }     
                else if (code_point_ex & BOTTOM)
                {
                    x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
                    y = ymax;
                }
                else if (code_point_ex & RIGHT)
                {
                    y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
                    x = xmax;
                }
                else
                {
                    y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
                    x = xmin;
                }
                //point 1 diluar, point 2 di dalem, simpen v1' dan v2
                if (code_point_ex == code_point1){
                    if(isFirst == 1){
                        clipped = createVectorPath(createVectorPoint(x-offsetX, y- offsetY));
                        appendToPath(clipped, createVectorPoint(x2-offsetX, y2 - offsetY));

                        isFirst = 0;

                    } else {
                        appendToPath(clipped, createVectorPoint(x-offsetX, y - offsetY));
                        appendToPath(clipped, createVectorPoint(x2-offsetX, y2 - offsetY));

                    }


                } else {
                    if(isFirst == 1){
                        clipped = createVectorPath(createVectorPoint(x-offsetX, y- offsetY));

                        isFirst = 0;

                    } else {
                        appendToPath(clipped, createVectorPoint(x-offsetX, y - offsetY));

                    }

                }


            }



            currentPoint = nextPoint;
            if (currentPoint[0] != NULL) {
                nextPoint = currentPoint[0]->nextPoint;
            }

        } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);

        
        if(clipped != NULL){
            if(clipped->numOfPoints > 2){
                enclosePath(clipped);
                checkForMinMaxUpdate(clipped);

                 return clipped;        

            }            
        }
        return NULL;

    }
    return NULL;

}

int getPointCode(int x, int y, double offsetX, double offsetY){
    int viewport_max_x = viewport_x + viewport_width;
    int viewport_max_y = viewport_y + viewport_height;
    int code = 0;
    if(x + offsetX < viewport_x){
        code = code + LEFT;
    }
    if(x + offsetX> viewport_max_x -1){
        code = code + RIGHT;
    }
    if(y + offsetY< viewport_y){
        code = code + TOP;
    }
    if(y + offsetY> viewport_max_y -1){
        code = code + BOTTOM;
    }
    return code;
}

int drawVectorPathClipping(VectorPath* path, unsigned int boundaryColor, unsigned int fillColor, int offsetX, int offsetY) {
    if (path != NULL) {
        VectorPath * toDraw = vectorClipping(path, offsetX, offsetY);
        if(toDraw != NULL){

            if (toDraw->firstPoint[0] != NULL && toDraw->firstPoint[0]->nextPoint[0] != NULL) {
                VectorPoint** currentPoint = toDraw->firstPoint;
                VectorPoint** nextPoint = toDraw->firstPoint[0]->nextPoint;

                do {
                    if (nextPoint[0] != NULL) {
                        drawVectorLine(currentPoint[0], currentPoint[0]->nextPoint[0], boundaryColor, offsetX, offsetY);
                    }


                    currentPoint = nextPoint;

                    if (currentPoint[0] != NULL) {
                        nextPoint = currentPoint[0]->nextPoint;
                    }
                } while (currentPoint[0] != NULL && currentPoint[0] != toDraw->firstPoint[0]);
            } else {
                return 0;
            }
            fillVector(toDraw, fillColor, boundaryColor, offsetX, offsetY);            
        } else{
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
    determineCriticalPoint(path);
    drawCritPoint(path, offsetX, offsetY, boundaryColor);

    if (checkIfPathIsClosed(path)) {
        for (int j = path->minY-1 + offsetY; j <= path->maxY + offsetY; j++) {
            isFilling = -1;
            for (int i = path->minX + offsetX -1; i <= path->maxX + offsetX; i++) {
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

void drawCircle(int x0, int y0, int radius, unsigned int boundaryColor, unsigned int fillColor) {
    unsigned int circleFrame[radius * 2][radius * 2];

    int isValidPointOnFrame(int x, int y) {
        if (x >= 0 && x < radius * 2/SCALE && y >=0 && y < radius * 2/SCALE)
            return 1;

        return 0;
    }

    void drawPixelOnFrame(int x, int y, unsigned int color) {
        int i = 0, j = 0;
        x = x*SCALE; y = y*SCALE;
        for (i = 0; i < SCALE; i++)
            for (j = 0; j < SCALE; j++) {
                circleFrame[x+i][y+j] = color;
            }
    }

    unsigned int getPixelColorOnFrame(int x, int y) {
        if(!isValidPointOnFrame(x,y)){
            return 0;
        }
        x = x * SCALE;
        y = y * SCALE;

       return circleFrame[x][y];
    }

    int isCircleCritPoint(int x, int y) {
        return ((y == 1 || y == (2 * radius) - 1));
    }

    int isOnBoundary(int x, int y) {
        return getPixelColorOnFrame(x, y) == boundaryColor;
    }
    void fillCircle() {
        int isFilling = -1;

        for (int j = 0; j <= radius * 2; j++) {
            isFilling = -1;
            for (int i = 0; i <= radius * 2; i++) {
                if (getPixelColorOnFrame(i, j)) {
                    while (isOnBoundary(i, j)) {
                        i++;
                    }
                    isFilling *= -1;
                }

                if (isCircleCritPoint(i, j)) { 
                    continue;
                }

                if (i < radius * 2) {
                    if (isFilling > 0) {
                        drawPixelOnFrame(i, j, fillColor);
                    }
                }

                // if (i == 0 || j == 0 || i == (radius * 2) - 1 || j == (radius * 2) - 1) {
                //     drawPixelOnFrame(i, j, rgbaToInt(255,0,0,0));
                // }
            }
        }
    }

    void drawCircle() {
        int minX = x0 - radius;
        int maxX = x0 + radius;
        int minY = y0 - radius;
        int maxY = y0 + radius;

        for (int x = 0; x < radius * 2; x++) {
            for (int y = 0; y < radius * 2; y++) {
                if (circleFrame[x][y] == boundaryColor || circleFrame[x][y] == fillColor) {
                    drawPixel(x + minX, y + minY, circleFrame[x][y]);
                }
            }
        }
    }

    void clearFrame() {
        for (int i = 0; i < radius * 2; i++) {
            for (int j = 0; j < radius * 2; j++) {
                unsigned int clearColor = rgbaToInt(0,0,0,0);
                if (boundaryColor == clearColor || fillColor == clearColor) {
                    clearColor = rgbaToInt(255,255,255,255);
                } else if (boundaryColor == rgbaToInt(255,255,255,255) || fillColor == rgbaToInt(255,255,255,255)) {
                    clearColor = rgbaToInt(0,255,255,255);
                }
                circleFrame[i][j] = clearColor;
            }
        }
    }

    int centerX = radius;
    int centerY = radius;
    int x = radius-1;
    int y = 0;
    int dx = 1;
    int dy = 1;
    int err = dx - (radius << 1);

    clearFrame();
    while (x >= y)
    {
        drawPixelOnFrame(centerX + x, centerY + y, boundaryColor);
        drawPixelOnFrame(centerX + y, centerY + x, boundaryColor);
        drawPixelOnFrame(centerX - y, centerY + x, boundaryColor);
        drawPixelOnFrame(centerX - x, centerY + y, boundaryColor);
        drawPixelOnFrame(centerX - x, centerY - y, boundaryColor);
        drawPixelOnFrame(centerX - y, centerY - x, boundaryColor);
        drawPixelOnFrame(centerX + y, centerY - x, boundaryColor);
        drawPixelOnFrame(centerX + x, centerY - y, boundaryColor);

        if (err <= 0)
        {
            y++;
            err += dy;
            dy += 2;
        }
        
        if (err > 0)
        {
            x--;
            dx += 2;
            err += dx - (radius << 1);
        }
    }

    fillCircle();
    drawCircle();
}