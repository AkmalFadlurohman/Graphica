#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "mouse.h"
#include "screen.h"
#include "bitmap_font.h"

#define WINDOW_WIDTH 900
#define WINDOW_HEIGHT 600
#define MOUSE_SPEED 6

const char *template_bitmap_font = "data/template_bitmap_font.io";

int main() {
    Screen *screen = initScreen();
    if (screen == 0) {
        return 0;
    }

    drawWindow(screen, WINDOW_WIDTH, WINDOW_HEIGHT);
    int window_x = (screen->width - WINDOW_WIDTH) / 2;
    int window_y = (screen->height - WINDOW_HEIGHT) / 2;

    Mouse *mouse = initMouse(window_x, window_y, window_x+WINDOW_WIDTH, window_y+WINDOW_HEIGHT, MOUSE_SPEED);
    if (mouse == 0) {
        return 0;
    }

    BitmapFont* bitmapFont = initBitmapFont(template_bitmap_font);
    drawBitmapChar(screen, bitmapFont, 500, 500, 'A');

    // while (1) {
    //     scanMouse(mouse);
    //     if(mouse->isEvent) {
    //         clearWindow(screen, WINDOW_WIDTH, WINDOW_HEIGHT);
    //         drawPointer(screen, mouse);
    //     } 
    // }

    return 0; 
}
