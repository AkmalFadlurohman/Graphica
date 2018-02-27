#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "mouse.h"
#include "screen.h"

int main() {
    Screen *screen = initScreen();
    if (screen == 0) {
        return 0;
    }

    Mouse *mouse = initMouse(screen->width-1, screen->height-1, 5);
    if (mouse == 0) {
        return 0;
    }
    
    while (1) {
        scanMouse(mouse);
        if(mouse->isEvent) {
            system("clear");
            drawPointer(screen, mouse->positionX, mouse->positionY);
        } 
    }

    return 0; 
}
