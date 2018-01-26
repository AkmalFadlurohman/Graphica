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
void drawPixel(int x, int y, unsigned int color);
void drawObject(struct f_Image* obj, int dir /*0 = right, 1 = left*/);
void drawLine(double x0, double y0, double x1, double x2);
void drawLineLow(double x0, double y0, double x1, double y1);
void drawLineHigh(double x0, double y0, double x1, double y1);
void drawLineEX(double x0, double y0, double x1, double y2);

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
    for (i=0; i<vinfo.yres/17; i++) {
      printf("\n");
    }

    /*Rendering starts here */

    char* fileName = "assets/plane.txt";

    struct f_Image* plane = f_loadImage(fileName);
    plane->posX = vinfo.xres;
    while(-(plane->posX) != plane->width ){
        plane->posX--;
        drawObject(plane, 1);
        usleep(1000);

    }

    f_freeImage(plane);

    drawLineEX(100, 100, 120, 110);
    drawLineEX(140, 130, 120, 150);
    drawLineEX(100, 100, 100, 200);
    drawLineEX(100, 170, 200, 170);

    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}

void tick(){

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
        drawPixel(x, y, rgbaToInt(0, 255, 0, 0));
        if (D > 0) {
            x += xi;
            D -= 2 * dy;
        }

        D += 2 * dx;
    }
}

void drawLineEX(double x0, double y0, double x1, double y1) {
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

void drawLine(double x0, double y0, double x1, double y1) {
    double deltaX = x1 - x0;
    double deltaY = y1 - y0;
    double deltaErr = abs(deltaY / deltaX);
    double error = 0;

    int y = y0;
    for (double x = x0; x < x1; x++) {
        drawPixel(x, y, rgbaToInt(255, 0, 0, 0));
        error += deltaErr;
        while (error >= 0.5) {
            y += (deltaY < 0 ? -1 : 1);
            error -= 1.0;
        }
    }
}