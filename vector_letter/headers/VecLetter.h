#ifndef VECLETTER_H
#define VECLETTER_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

struct Point {
	double x;
	double y;
};

struct Line {
	struct Point* points[2];
};

struct VecLetter {
	int posX;
	int posY;
	int height;
	int width;

	int numOfLines;
	int numOfCritPoints;

	struct Line** lines;
	struct Point** critPoints;
};

struct Point* pointInit(double x, double y);
struct Line* lineInit(double x1, double y1, double x2, double y2);
//struct VecLetter* vecLetterInit(int x, int y, int w, int h, int l, int c, struct Line** arrLines, struct Point** arrPoints);
struct VecLetter* vecLetterInit(int x, int y, int w, int h, int l, int c);
void freePoint(struct Point* point);
void freeLine(struct Line* line);
void freeVecLetter(struct VecLetter* letter);

#endif
