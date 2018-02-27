#ifndef MOUSE_H
#define MOUSE_H

#include <stdlib.h>

typedef struct Screen {
    int scale;
    int width;
    int height;
} Screen;


Screen* initScreen() {
    Screen* s = (Screen*) malloc(sizeof(Screen));
}

void drawPixel(int x, int y);

void drawPointer(int x, int y);


#endif