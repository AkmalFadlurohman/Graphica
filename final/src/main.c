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

void drawMainMenu(Screen* s, BitmapFont* bf, int x, int y);

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

    while (1) {
        scanMouse(mouse);
        if(mouse->isEvent) {
            clearWindow(screen, WINDOW_WIDTH, WINDOW_HEIGHT);
            drawMainMenu(screen, bitmapFont, window_x, window_y);
            drawPointer(screen, mouse);
            
            if (mouse->isRightClick)
                printf("Right click event\n");
            if (mouse->isLeftClick)
                printf("Left click event\n");
        } 
    }

    return 0; 
}


void drawMainMenu(Screen* s, BitmapFont* bf, int x, int y) {
    int line_height_5 = bf->char_height*5;
    int line_height_3 = bf->char_height*3;
    drawBitmapString(s, bf, x, y, "MENU UTAMA", 5);
    drawBitmapString(s, bf, x, y + 2*line_height_5, "1. FONT", 3);
    drawBitmapString(s, bf, x, y + 2*line_height_5 + line_height_3, "2. ANIMASI PESAWAT", 3);
    drawBitmapString(s, bf, x, y + 2*line_height_5 + 2*line_height_3, "3. PETA ITB", 3);
    drawBitmapString(s, bf, x, y + 2*line_height_5 + 4*line_height_3, "KETIK NOMOR MENU LALU TEKAN ENTER", 2);
}