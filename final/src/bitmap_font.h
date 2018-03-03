#ifndef BITMAP_FONT_H
#define BITMAP_FONT_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include "screen.h"

#define SCALE 2
#define WHITE (rgbaToInt(255,255,255,0))
#define BLACK (rgbaToInt(0,0,0,0))
#define RED rgbaToInt(255, 0, 0, 0)

typedef struct BitmapFont {
    int char_height;    // Character height in pixel, same for all char
    int num_of_char;    // Number of available char
    char *char_index;   // Array of char
    int *char_width;    // Array of char size, can be accessed with index
    char **font;        // Map of pixel for every character
} BitmapFont;

BitmapFont* initBitmapFont(const char *filename) {
    BitmapFont* bf = (BitmapFont*) malloc(sizeof(BitmapFont));
    
    bf->char_height = 0;
    bf->num_of_char = 0;
    bf->char_index = 0;
    bf->char_width = 0;
    bf->font = 0;

    FILE *fptr;
    size_t buffer_size = 80;
    char *buffer = malloc(buffer_size * sizeof(char));
    int i = 0, ret = 0;

    // Read from external file: filename
    fptr = fopen(filename, "r");
    if (fptr == NULL){
        perror("Cannot open file \n");
        exit(0);
    }

    ret = fscanf(fptr, "%d %d\n", &bf->num_of_char, &bf->char_height);
    if (ret != 2) {
        perror ("Failed to read font specification!\n");
    } else {
        printf("num_of_char: %d\n", bf->num_of_char);
        printf("char_height: %d\n", bf->char_height);
    }
    bf->char_index = (char*) malloc(bf->num_of_char * sizeof(char));
    bf->font = (char**) malloc(bf->num_of_char * sizeof(char*));
    bf->char_width = (int*) malloc(bf->num_of_char * sizeof(int));

    while (-1 != getline(&buffer, &buffer_size, fptr) && 1 < bf->num_of_char) {
        (bf->font)[i] = (char*) malloc(buffer_size * sizeof(char));
        ret = 0; ret = sscanf(buffer, "%c|%d|%[^\n]s", &bf->char_index[i], &bf->char_width[i], bf->font[i]);
        if (ret != 3) {
            printf("Error reading line %d, only read %d\n", i+2, ret);
        }
        i++;
    }

    free(buffer);
    fclose(fptr);

    return bf;
}

int getBitmapCharIndex(BitmapFont *bf, char c) {
    int i = 0;
    for (i = 0; i < bf->num_of_char; i++) {
        if (bf->char_index[i] == c)
            return i;
    }
    return -1;
}

int drawBitmapChar(Screen *s, BitmapFont *bf, int x, int y, char c, int scale) {
    int i = 0, j = 0, k = 0;
    int idx = getBitmapCharIndex(bf, c);
    int width = 4, pixel_length = 0;
    char* pixel = 0;

    if (idx == -1) {
        return 0;
    }
    
    width = bf->char_width[idx];
    pixel = bf->font[idx];
    pixel_length = strlen(pixel);

    x = x / scale; y = y / scale;

    for (j = 0; j < bf->char_height+2; j++)
        for (i = 0; i < width+2; i++) {
            if ((i == 0) || (i == width+1) || (j == 0) || (j == bf->char_height+1))
                continue;
            else {
                if (k >= pixel_length) {
                    continue;
                }
                if (pixel[k] == '1')
                    drawPixelWithScale(s, i+x, j+y, WHITE, scale);
                k++;
            }
        }

    return (width + 2) * scale;
}

void drawBitmapString(Screen *s, BitmapFont *bf, int x, int y, char* text, int scale) {
    int len = strlen(text);
    for (int i = 0; i < len; i++) {
        x += drawBitmapChar(s,bf,x,y,text[i], scale);
    }
}

#endif