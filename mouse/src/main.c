#include "headers/fbp.h"

#define MOUSE_SPEED 12

const char *template_bitmap_font = "data/template_bitmap_font.io";

int window_x;
int window_y;
int max_window_x;
int max_window_y;

Mouse *mouse;

int main(int argc, char **argv) {
	initializeFBP();
    viewport_x = 0;
    viewport_y = 0;

    window_x = (vinfo.xres - viewport_width) / 2;
    window_y = (vinfo.yres - viewport_height) / 2;
    max_window_x = window_x + viewport_width;
    max_window_y = window_y + viewport_height;

    mouse = initMouse(window_x, window_y, max_window_x, max_window_y, MOUSE_SPEED);
    if (mouse == 0) {
        return 0;
    }

    BitmapFont* bitmapFont = initBitmapFont(template_bitmap_font);

    critColor = rgbaToInt(250,250,250,0);
    frameColor = rgbaToInt(247,247,247,0);

    
    while(RUNNING) {
        scanMouse(mouse);
        if(mouse->isEvent) {
            clearScreen();
            render();
            drawPointer(mouse);
            if (mouse->isRightClick) {
                clearViewPort(rgbaToInt(0,0,0,0));
            }
            if (mouse->isLeftClick) {
                drawPointAfterClicked(mouse);
                
            }
        }
        
    }
    munmap(fbp, screensize);
    close(fbfd);
}
