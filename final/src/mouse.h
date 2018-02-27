#ifndef MOUSE_H
#define MOUSE_H

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

const char *device_mouse = "/dev/input/mice";

typedef struct Mouse {
    int fd;                 // required for reading mouse event data
    int screen_max_x;       // screen width
    int screen_max_y;       // screen height

    int speed;
    int isEvent;            // isEvent is active if current state is different from previous
    int positionX;          // mouse current position in x coordinate
    int positionY;          // mouse current position in y coordinate
    int isRightClick;
    int isLeftClick;
} Mouse;

Mouse* initMouse(int screen_max_x, int screen_max_y, int speed) {
    Mouse* m = (Mouse*) malloc(sizeof(Mouse));
    
    m->fd = open(device_mouse, O_RDWR);
    if(m->fd == -1) {
        printf("ERROR Opening %s\n", device_mouse);
        return 0;
    }

    m->screen_max_x = screen_max_x;
    m->screen_max_y = screen_max_y;
    m->speed = speed;

    m->isEvent = 0;
    m->positionX = 0;
    m->positionY = 0;
    m->isRightClick = 0;
    m->isLeftClick = 0;

    return m;
}

void scanMouse(Mouse* m) {
    int bytes, left, right, middle;
    signed char x, y;
    unsigned char data[3];

    bytes = read(m->fd, data, sizeof(data));

    if(bytes > 0) {
        left = data[0] & 0x1;
        right = data[0] & 0x2;
        middle = data[0] & 0x4;

        x = data[1];
        y = data[2];
        
        // printf("x=%d, y=%d, left=%d, middle=%d, right=%d\n", x, y, left, middle, right);
        if (x < 0)
            m->positionX -= m->speed;
        if (x > 0)
            m->positionX += m->speed;
        if (y > 0)
            m->positionY -= m->speed;
        if (y < 0)
            m->positionY += m->speed;

        if (m->positionY < 0) m->positionY = 0;
        if (m->positionX < 0) m->positionX = 0;
        if (m->positionY > m->screen_max_y) m->positionY = m->screen_max_y;
        if (m->positionX > m->screen_max_x) m->positionX = m->screen_max_x;

        if (right > 0) m->isRightClick = 1; else m->isRightClick = 0;
        if (left > 0) m->isLeftClick = 1; else m->isLeftClick = 0;

        m->isEvent = 1;
    } else {
        m->isEvent = 0;
    }
}

#endif