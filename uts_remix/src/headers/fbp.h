#ifndef FBP_H
#define FBP_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <math.h>
#include "VectorPath.h"
#include "VecLetter.h"
#include "f_image.h"

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

#define ESC 27

// Mouse structure
typedef struct Mouse {
    int fd;                 // required for reading mouse event data
    int screen_max_x;
    int screen_max_y;
    int screen_min_x;
    int screen_min_y;

    int speed;
    int isEvent;            // isEvent is active if current state is different from previous
    int positionX;          // mouse current position in x coordinate
    int positionY;          // mouse current position in y coordinate
    int isRightClick;
    int isLeftClick;
} Mouse;

// Bitmap Font
typedef struct BitmapFont {
    int char_height;    // Character height in pixel, same for all char
    int num_of_char;    // Number of available char
    char *char_index;   // Array of char
    int *char_width;    // Array of char size, can be accessed with index
    char **font;        // Map of pixel for every character
} BitmapFont;

// Final Functions
void drawPointer(Mouse* m);

//Game World
extern unsigned int world[WORLD_WIDTH][WORLD_HEIGHT]; 
extern int viewport_x;
extern int viewport_y;
extern int viewport_width;
extern int viewport_height;

//FBP variables
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;

extern char *fbp;
extern int centerX;
extern int fullY;
extern int frameColor;
extern int critColor;
extern long int screensize;
extern int fbfd;

//VecLetter variables
extern int letterCount, letterHeight;
extern struct VecLetter** letters;
extern int COLOR;
extern int BORDER_COLOR;
extern int MARGIN_VERTICAL;
extern int MARGIN_HORIZONTAL;

//Bitmap plane variables

// F1 | FBP manipulations
void initializeFBP();
void clearViewPort(int color);
void clearScreen();
void render();
void drawPixel(int x, int y, unsigned int color);
unsigned int rgbaToInt(int r, int g, int b, int a);
unsigned int getPixelColor(int x, int y);

// F2-A | Bitmap plane functions
void drawPixelWithScale(int x, int y, unsigned int color, int scale);
void drawObject(struct f_Image* obj, int dir /*0 = right, 1 = left*/);
void f_drawPixel(int x, int y, unsigned int color);

// F2-B | Line drawing functions
int isValidPoint(int x, int y);
void drawLineLow(double x0, double y0, double x1, double y1, unsigned int color);
void drawLineHigh(double x0, double y0, double x1, double y1, unsigned int color);
void drawVectorLine(VectorPoint* point1, VectorPoint* point2, unsigned int color, int offsetX, int offsetY);

// F2-C | VecLetters drawing Functions
int isValidPointScale(int x, int y, int scale);
void drawLine_line(struct Line* line, int color, int offsetX, int offsetY);
void drawLineWithScale(double x0, double y0, double x1, double y2, int color, int Scale);
void drawLineHighWithScale(double x0, double y0, double x1, double y1, int color, int scale);
void drawLineLowWithScale(double x0, double y0, double x1, double y1, int color, int scale);
void loadLetters(char* fileName);
int drawLetters(char c, int* x, int* y);
void fillLetter(struct VecLetter *vecletter, unsigned int color, unsigned int boundaryColor, int offsetX, int offsetY);

// F2-D | Bitmap line functions
void drawLine(double x0, double y0, double x1, double y2, int color);
void drawLaser(int x0, int y0, int dx, int dy, int scale, int* pt);

// F3 | VectorPath exclusive functions
int getPointCode(int x, int y, double offsetX, double offsetY);
VectorPath * vectorClipping (VectorPath * path, int offsetX, int offsetY);
int drawVectorPathClipping(VectorPath* path, unsigned int boundaryColor, unsigned int fillColor, int offsetX, int offsetY);
int drawVectorPath(VectorPath* path, unsigned int boundaryColor, unsigned int color, int offsetX, int offsetY);
void drawCritPoint(VectorPath* path, int offsetX, int offsetY, unsigned int boundaryColor);
int rotatePath(VectorPath* path, float degree, int originX, int originY);
int dilatatePath(VectorPath* path, int originX, int originY, float zoom);
int translatePath(VectorPath* path, int dx, int dy);
void fillVector(VectorPath* path, unsigned int fillColor, unsigned int boundaryColor, int offsetX, int offsetY);
void determineCriticalPoint(VectorPath* vecPath);

// F4 | Circle functions
void drawCircle(int x0, int y0, int radius, unsigned int boundaryColor, unsigned int fillColor);

// F5 | vector plane functions
int drawVector(char c, int x, int y, unsigned int border_color, unsigned int fill_color, float degree, int originX, int originY, float zoom);
int isCritPoint(int i, int j, unsigned int boundaryColor);
void fillPlane(struct VecLetter* vecletter, unsigned int color, unsigned int boundaryColor, int minX, int minY, int maxX, int maxY);


// Bitfont

BitmapFont* initBitmapFont(const char *filename);

int getBitmapCharIndex(BitmapFont *bf, char c);

int drawBitmapChar(BitmapFont *bf, int x, int y, char c, int scale);

void drawBitmapString(BitmapFont *bf, int x, int y, char* text, int scale);

// Mouse
Mouse* initMouse(int screen_min_x, int screen_min_y, int screen_max_x, int screen_max_y, int speed);

void scanMouse(Mouse* m);


#endif