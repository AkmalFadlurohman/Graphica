#ifndef VECTOR_POINT_H
#define VECTOR_POINT_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

struct VectorPoint {
	struct VectorPoint **prevPoint; 
	struct VectorPoint **nextPoint; 
	double x;
	double y;
};

typedef struct VectorPoint VectorPoint;

// Allocate memory for VectorPoints without any links to other VectorPoints
VectorPoint* createVectorPoint(double _x, double _y);
// Free memory used by a VectorPoint
void freeVectorPoint(VectorPoint* vectorPoint);

// Set the next connected point of a VectorPoint
void setNextPoint(VectorPoint* currentPoint, VectorPoint* _nextPoint);
// Set the previous connected point of a VectorPoint
void setPrevPoint(VectorPoint* currentPoint, VectorPoint* _prevPoint);

#endif
