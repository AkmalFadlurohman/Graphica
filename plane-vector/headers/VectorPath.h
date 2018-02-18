#ifndef VECTOR_PATH_H
#define VECTOR_PATH_H

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "VectorPoint.h"

typedef struct VectorPath {
	struct VectorPoint **firstPoint;
	struct VectorPoint **lastPoint;
	struct VectorPoint **criticalPoints;
	int minX;
	int minY;
	int maxX;
	int maxY;
	int numOfPoints;
} VectorPath;

// Allocates memory for a VectorPath. The lastPoint member variable will be determined by the firstPoint path.
VectorPath* createVectorPath(struct VectorPoint* _firstPoint);
// Creates VectorPath based on external text file
VectorPath* createVectorPathFromFile(char* fileName);
// Frees memory of a VectorPath along with all the points it contains
void freeVectorPath(VectorPath* vectorPath);

// Append a VectorPoint at the end of a path (becomes the new last point)
void appendToPath(VectorPath* vectorPath, struct VectorPoint* vectorPoint);
// Append a VectorPoint at the beginning of a path (becomes the new first point)
void prependToPath(VectorPath* vectorPath, struct VectorPoint* vectorPoint);
// Insert a point after a certain point in a path given it actually exists
void insertToPath(VectorPath* vectorPath, struct VectorPoint* targetPoint, struct VectorPoint* newPoint);

// Removes the first point in a path (changes the first point pointer if one exists)
void removeFirstPoint(VectorPath* vectorPath);
// Removes the last point in a path (changes the last point pointer if one exists)
void removeLastPoint(VectorPath* vectorPath);
// Removes a certain point of a path if the address provided exists in the path (the path will stay connected if possible)
void removePoint(VectorPath* vectorPath, struct VectorPoint* targetPoint);

// Enclose a path if possible
void enclosePath(VectorPath* vectorPath);
// Returns 1 if path is closed, 0 otherwise
int checkIfPathIsClosed(VectorPath* vectorPath);
// Returns 1 if specified point address is a member of the path, 0 if not found, and -1 if an error occured
int isPathMember(VectorPath* path, struct VectorPoint* wantedPoint);
// Return the address of point with the same coordinate specified if exist, NULL otherwise
struct VectorPoint* findPathMemberByCoordinate(VectorPath* path, float _x, float _y);
#endif
