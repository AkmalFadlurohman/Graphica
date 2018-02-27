#ifndef SCREEN_H
#define SCREEN_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

const char *device_screen = "/dev/fb0";
const int cursor_size = 10;

typedef struct Screen {
    int width;
    int height;
    
    char *fbp;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
} Screen;


Screen* initScreen() {
    Screen* s = (Screen*) malloc(sizeof(Screen));

    int fbfd = 0;
    long int screensize = 0;
    
    // Open the file for reading and writing
    fbfd = open(device_screen, O_RDWR);
    if (fbfd == -1) {
        printf("ERROR Opening %s\n", device_screen);
        return 0;
    }

        // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &(s->finfo)) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }

    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &(s->vinfo)) == -1) {
        perror("Error reading variable information");
        exit(3);
    }
    printf("Detected display: %dx%d, %dbpp\n", s->vinfo.xres, s->vinfo.yres, s->vinfo.bits_per_pixel);

    // Figure out the size of the screen in bytes
    screensize = s->vinfo.xres * s->vinfo.yres * s->vinfo.bits_per_pixel / 8;

    // Map the device to memory
    s->fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((int)s->fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        exit(4);
    }

    s->width = s->vinfo.xres;
    s->height = s->vinfo.yres;

    return s;
}

void drawPixel(Screen* s, int x, int y, unsigned int color) {
    long int location;
            
    location = (x+s->vinfo.xoffset) * (s->vinfo.bits_per_pixel/8) + (y+s->vinfo.yoffset) * s->finfo.line_length;
    *(s->fbp + location) = color;
    *(s->fbp + location + 1) = color >> 8;
    *(s->fbp + location + 2) = color >> 16;
    *(s->fbp + location + 3) = color >> 24;
}

unsigned int rgbaToInt(int r, int g, int b, int a) {
    return a << 24 | r << 16 | g << 8 | b;
}


void drawPointer(Screen* s, int x, int y) {
    int i = x; int j = y;
    int ie = x+cursor_size; int je = j+cursor_size;
    if (ie >= s->width) ie = s->width-1;
    if (je >= s->height) je = s->height-1;
    for (int i = x; i < ie; i++)
        for (int j = y; j < je; j++)
            drawPixel(s, i, j, rgbaToInt(255,0,0,0));
}

#endif