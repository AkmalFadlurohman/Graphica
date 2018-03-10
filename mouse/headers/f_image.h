#ifndef F_IMAGE_H
#define F_IMAGE_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/ioctl.h>


struct f_Pixel {
	unsigned int color;
};

struct f_Image {
	int posX;
	int posY;
	int width;
	int height;
	struct f_Pixel** bitmap;	
};

unsigned int f_rgbaToInt(int r, int g, int b, int a);
struct f_Pixel* f_initPixel(int r, int g, int b, int a);
void f_modPixel(struct f_Pixel* pixel, int r, int g, int b, int a);
void f_freePixel(struct f_Pixel* pixel);
struct f_Image* f_initImage(size_t width,size_t height);
void f_freeImage(struct f_Image* image);

struct f_Image* f_loadImage(char* fileName);

#endif