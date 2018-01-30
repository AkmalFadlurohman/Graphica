#ifndef F_VECLETTER_H
#define F_VECLETTER_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

struct f_Point {
	double x;
	double y;
};

struct f_Line {
	struct f_Point* points[2];
};

struct f_VecLetter {
	int posX;
	int posY;
	int height;
	int width;

	int numOfLines;
	int numOfCritPoints;

	struct f_Line** lines;
	struct f_Point** critPoints;
};

struct f_Point* f_pointInit(double x, double y);
struct f_Line* f_lineInit(double x1, double y1, double x2, double y2);
struct f_VecLetter* f_vecLetterInit(int x, int y, int w, int h, int l, int c, struct f_Line** arrLines, struct f_Point** arrPoints);

void f_freePoint(struct f_Point* point);
void f_freeLine(struct f_Line* line);
void f_freeVecLetter(struct f_VecLetter* letter);

// TODO: 
struct f_VecLetter** f_loadLetters(char* fileName);

#endif