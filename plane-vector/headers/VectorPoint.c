#include "VectorPoint.h"

VectorPoint* createVectorPoint(double _x, double _y) {
	VectorPoint* newPoint = malloc(sizeof(VectorPoint));

	if (newPoint == NULL) {
		printf("[VectorPoint.c => createVectorPoint()] Failed to allocate memory for VectorPoint");
		return NULL;
	}

	newPoint->x = _x;
	newPoint->y = _y;
	newPoint->nextPoint = malloc(sizeof(VectorPoint*));
	if (newPoint->nextPoint == NULL) {
		printf("[VectorPoint.c => createVectorPoint()] Failed to allocate memory for nextPoint pointer");
		free(newPoint);
		return NULL;
	}

	newPoint->prevPoint = malloc(sizeof(VectorPoint*));
	if (newPoint->prevPoint == NULL) {
		printf("[VectorPoint.c => createVectorPoint()] Failed to allocate memory for prevPoint pointer");
		free(newPoint->nextPoint);
		free(newPoint);
		return NULL;
	}

	newPoint->nextPoint[0] = NULL;
	newPoint->prevPoint[0] = NULL;

	return newPoint;
}

void freeVectorPoint(VectorPoint* vectorPoint) {
	if (vectorPoint != NULL) {
		if (vectorPoint->prevPoint[0] != NULL) {
			vectorPoint->prevPoint[0]->nextPoint[0] = NULL;
			free(vectorPoint->prevPoint);
		}

		if (vectorPoint->nextPoint[0] != NULL) {
			vectorPoint->nextPoint[0]->prevPoint[0] = NULL;
			free(vectorPoint->nextPoint);
		}
		free(vectorPoint);

		printf("successfully freed point\n");
	}
}

void setNextPoint(VectorPoint* currentPoint, VectorPoint* _nextPoint) {
	if (currentPoint == NULL) {
		printf("[VectorPoint.c => setNextPoint()] The current point has a value of NULL\n");
		return;
	} else if (_nextPoint == NULL) {
		printf("[VectorPoint.c => setNextPoint()] The next point has a value of NULL\n");
		return;
	}

	currentPoint->nextPoint[0] = _nextPoint;
	_nextPoint->prevPoint[0] = currentPoint;
}

void setPrevPoint(VectorPoint* currentPoint, VectorPoint* _prevPoint) {
	if (currentPoint == NULL) {
		printf("[VectorPoint.c => setPrevPoint()] The current point has a value of NULL");
		return;
	} else if (_prevPoint == NULL) {
		printf("[VectorPoint.c => setPrevPoint()] The previous point has a value of NULL");
		return;
	}

	currentPoint->prevPoint[0] = _prevPoint;
	_prevPoint->nextPoint[0] = currentPoint;

}