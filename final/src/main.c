#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include "mouse.h"

// Draw mouse pointer in coordinat x,y
void drawPointer(int x, int y);

const char *device_monitor = "/dev/gb0";

int main() {
    Mouse *mouse = initMouse(500,500);
    
    while (1) {
        scanMouse(mouse);
        if(mouse->isEvent) {
            printf("x=%d, y=%d, left=%d, right=%d\n", 
                mouse->positionX, 
                mouse->positionY, 
                mouse->isLeftClick, 
                mouse->isRightClick);
        } 
    }

    return 0; 
}


void drawPointer(int x, int y) {

}