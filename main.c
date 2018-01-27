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

#define SCALE 2

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

char *fbp = 0;
void tick();
int isValidPoint(int x, int y);
void drawPixel(int x, int y, unsigned int color);
void drawObject(struct f_Image* obj, int dir /*0 = right, 1 = left*/);
void drawLineLow(double x0, double y0, double x1, double y1);
void drawLineHigh(double x0, double y0, double x1, double y1);
void drawLine(double x0, double y0, double x1, double y2);
void drawTravel(int x0, int y0, int dx, int dy, int t);

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
    system("clear");
    // for (i=0; i<vinfo.yres/17; i++) {
    //   printf("\n");
    // }

    /*Rendering starts here */
    char* fileName = "assets/plane.txt";

    struct f_Image* plane = f_loadImage(fileName);
    plane->posX = vinfo.xres;
    int t = 0, centerX = vinfo.xres/2/SCALE, bottomY = vinfo.yres/2 - 1;
    printf("Bottom Y: %d\n", vinfo.yres/SCALE);
    drawPixel(centerX, bottomY, rgbaToInt(255,0,0,0));
    int delay = 0;
    while(1){

        plane->posX--;
        if (plane->posX < -plane->width) {
            plane->posX = vinfo.xres;
            plane->posY += 7;
        }
        drawObject(plane, 1);
        usleep(3000);
        drawTravel(centerX, bottomY, 10, -10, t);
        drawTravel(centerX, bottomY, 5, -3, t);
        drawTravel(centerX, bottomY, 0, -10, t);
        drawTravel(centerX, bottomY, -10, -10, t);
        drawTravel(centerX, bottomY, -5, -10, t);
        drawTravel(centerX, bottomY, -0, -2, t);
        if (delay % 10 == 0) {
            t++;
        }
        delay++;
    }

    f_freeImage(plane);


    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}

int isValidPoint(int x, int y) {
    if (x >= 0 && x < vinfo.xres && y >=0 && y < vinfo.yres)
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

void drawLineLow(double x0, double y0, double x1, double y1) {
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
        drawPixel(x, y, rgbaToInt(255, 0, 0, 0));
        if (D > 0) {
            y += yi;
            D -= 2 * dx;
        }
        D += 2 * dy;
    }
}

void drawLineHigh(double x0, double y0, double x1, double y1) {
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
        drawPixel(x, y, rgbaToInt(0, 255, 0, 0));
        if (D > 0) {
            x += xi;
            D -= 2 * dy;
        }

        D += 2 * dx;
    }
}

void drawLine(double x0, double y0, double x1, double y1) {
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            drawLineLow(x1, y1, x0, y0);
        } else {
            drawLineLow(x0, y0, x1, y1);
        }
    } else {
        if (y0 > y1) {
            drawLineHigh(x1, y1, x0, y0);
        } else {
            drawLineHigh(x0, y0, x1, y1);
        }
    }
}

void drawTravel(int x0, int y0, int dx, int dy, int t) {
    drawLine(x0, y0, x0 + dx * t, y0 + dy * t);
}