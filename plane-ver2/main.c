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
#include "headers/f_image.h"
#include <math.h>

#define SCALE 1

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

char *fbp = 0;
void tick();
int isValidPoint(int x, int y);
int isValidPointScale(int x, int y, int scale);
void drawPixel(int x, int y, unsigned int color);
void drawPixelWithScale(int x, int y, unsigned int color, int scale);
void drawObject(struct f_Image* obj, int dir /*0 = right, 1 = left*/);
void drawLineLow(double x0, double y0, double x1, double y1, int color);
void drawLineLowWithScale(double x0, double y0, double x1, double y1, int color, int scale);
void drawLineHigh(double x0, double y0, double x1, double y1, int color);
void drawLineHighWithScale(double x0, double y0, double x1, double y1, int color, int scale);
void drawLine(double x0, double y0, double x1, double y2, int color);
void drawLineWithScale(double x0, double y0, double x1, double y2, int color, int Scale);
void drawLaser(int x0, int y0, int dx, int dy, int scale, int* pt);
void drawObjectWithRotation(struct f_Image* obj, int dir /*0 = right, 1 = left*/, int scale, double degree);
void clear(int color);
int centerX = 0;
int fullY = 0;

int main() {

    int fbfd = 0;
    long int screensize = 0;
    int x = 0, y = 0, i = 0;
    char text[256];

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

    /*Render and start animation*/
    char* fileName = "assets/plane.txt";

    struct f_Image* plane = f_loadImage(fileName);
    plane->posX = vinfo.xres;
    plane->posY = 100;

    
    system("clear");
    int scale = 1;
    int degree = 0;
    int scaleSpeed = 0;
     while (1) {
            clear(rgbaToInt(0,0,0,0));
            plane->posX-=5;
            if (plane->posX < -plane->width) {
                plane->posX = vinfo.xres;
                plane->posY += 7;
            }
            scaleSpeed ++;
            if(scaleSpeed > 10){
                scaleSpeed = 0;
                scale++;
                if(scale >4){
                    scale = 1;
                }

            }

           drawObjectWithRotation(plane, 1,scale,degree);
           degree+=10;
            
         usleep(30000);
     }

    f_freeImage(plane);


    munmap(fbp, screensize);
    close(fbfd);
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
            if(x+i < vinfo.xres && x+i >= 0 && y+j <vinfo.yres && y+j >=0){
                location = (x+i+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+j+vinfo.yoffset) * finfo.line_length;
                *(fbp + location) = color;
                *(fbp + location + 1) = color >> 8;
                *(fbp + location + 2) = color >> 16;
                *(fbp + location + 3) = color >> 24;
            }

        }
}
void drawPixelWithScaleOrigin(int x, int y, int oriX, int oriY, unsigned int color, int scale) {
    long int location;
    int i = 0, j = 0;
    x = (x-oriX)*scale + oriX; y = (y-oriY)*scale+oriY;
    for (i = 0; i < scale; i++)
        for (j = 0; j < scale; j++) {
            if(x+i < vinfo.xres && x+i >= 0 && y+j <vinfo.yres && y+j >=0){
                location = (x+i+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+j+vinfo.yoffset) * finfo.line_length;
                *(fbp + location) = color;
                *(fbp + location + 1) = color >> 8;
                *(fbp + location + 2) = color >> 16;
                *(fbp + location + 3) = color >> 24;
            }
        }
}
void drawObject(struct f_Image* obj, int dir /*0 = right, 1 = left*/){
    for (int i = 0; i <  obj->height; i++) {
        for (int j = 0; j < obj->width ; j++){
                if(dir == 0){
                    if(j+obj->posX < vinfo.xres && j+obj->posX >= 0 && i+obj->posY <vinfo.yres && i+obj->posY  >=0)
                        drawPixel(j+obj->posX,i+obj->posY,obj->bitmap[i*obj->width+j]->color);

                } else{
                   if(obj->width-j + obj->posX < vinfo.xres && obj->width-j + obj->posX >= 0 && i +obj->posY <vinfo.yres && i +obj->posY >=0)
                        drawPixel(obj->width-j + obj->posX,i +obj->posY,obj->bitmap[i*obj->width+j]->color);
                }
            
        }
    }
}
void drawObjectWithRotation(struct f_Image* obj, int dir /*0 = right, 1 = left*/, int scale, double degree){

    double originX = obj->posX + obj->width/2;
    double originY = obj->posY + obj->height/2;

    for (int i = 0; i <  obj->height; i++) {
        for (int j = 0; j < obj->width ; j++){
                int x, y;
                if(dir == 0){
                    x = j+obj->posX;
                    y = i+obj->posY;


                } else{
                    x = obj->width-j + obj->posX;
                    y = i +obj->posY;

                   
                }
                if(x < vinfo.xres && x >= 0 && y <vinfo.yres && y >=0){
                    int color = obj->bitmap[i*obj->width+j]->color;
                    double x1,y1;
                    x1 = (double) x;
                    y1 = (double) y;
                    double t=(22*degree)/(180*7);
                    double b1=((x1-originX)*cos(t))-((y1-originY)*sin(t)) + originX;
                     double b2=((x1-originX)*sin(t))+((y1-originY)*cos(t)) +originY;
                    x = (int) b1;
                    y = (int) b2;
                    drawPixelWithScaleOrigin(x,y,originX,originY,color,scale);
                }


            
        }
    }
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