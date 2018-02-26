#include "VectorPath.h"

int VectorPath_debug = 0;

int fpeek(FILE * const fp)
{
  const int c = getc(fp);
  return c == EOF ? EOF : ungetc(c, fp);
}

VectorPath* createVectorPath(VectorPoint* _firstPoint) {
	VectorPath* newPath = malloc(sizeof(VectorPath));

	if(_firstPoint == NULL) {
		printf("[VectorPath.c => createVectorPath()] Initial VectorPoint (currently) cannot be NULL\n");
		return NULL;
	}

	if (newPath == NULL) {
		printf("[VectorPath.c => createVectorPath()] Failed to allocate memory for VectorPath\n");
		return NULL;
	}

	newPath->firstPoint = malloc(sizeof(VectorPoint*));
	if (newPath->firstPoint == NULL) {
		printf("[VectorPath.c => createVectorPath()] Failed to allocate memory for firstPoint pointer");
		free(newPath);
		return NULL;
	}

	newPath->lastPoint = malloc(sizeof(VectorPoint*));
	if (newPath->lastPoint == NULL) {
		printf("[VectorPath.c => createVectorPath()] Failed to allocate memory for lastPoint pointer");
		free(newPath->firstPoint);
		free(newPath);
		return NULL;
	}

	newPath->firstPoint[0] = _firstPoint;
	
	newPath->maxX = _firstPoint->x;
	newPath->maxY = _firstPoint->y;

	newPath->minX = _firstPoint->x;
	newPath->minY = _firstPoint->y;

	newPath->numOfPoints = 1;

	if (_firstPoint != NULL) {
		VectorPoint*_lastPoint = newPath->firstPoint[0];
		while (_lastPoint->nextPoint[0] != NULL && _lastPoint->nextPoint[0] != newPath->firstPoint[0]) {
			_lastPoint = _lastPoint->nextPoint[0];

			if (newPath->maxX < _lastPoint->x) {
				newPath->maxX = _lastPoint->x;
			}
			if (newPath->maxY < _lastPoint->y) {
				newPath->maxY = _lastPoint->y;
			}

			if (newPath->minX > _lastPoint->x) {
				newPath->minX = _lastPoint->x;
			}
			if (newPath->minY > _lastPoint->y) {
				newPath->minY = _lastPoint->y;
			}

			newPath->numOfPoints++;
		}

		newPath->lastPoint[0] = _lastPoint;
	}

	return newPath;
}

VectorPath* createVectorPathFromFile(char* fileName) {
	FILE* externalFile;
	externalFile = fopen(fileName, "r");

	if (externalFile == NULL) {
		printf("[VectorPath.c => createVectorPathFromFile()] Failed to open external file\n");
		return NULL;
	}

	VectorPath* newPath;

	float _x, _y;
	
	if (fscanf(externalFile, "%f,%f\n", &_x, &_y) != 2) {
		printf("[VectorPath.c => createVectorPathFromFile()] Failed to parse the file. Please check the format\n");
	} else {
		newPath = createVectorPath(createVectorPoint(_x, _y));
		// appendToPath(newPath, createVectorPoint(_x, _y));
	}

	while (fscanf(externalFile, "%f,%f\n", &_x, &_y) == 2) {
		newPath->numOfPoints++;
		if (newPath->maxX < _x) {
			newPath->maxX = _x;
		}
		if (newPath->maxY < _y) {
			newPath->maxY = _y;
		}

		if (newPath->minX > _x) {
			newPath->minX = _x;
		}
		if (newPath->minY > _y) {
			newPath->minY = _y;
		}

		if (fpeek(externalFile) == ';' && newPath->firstPoint[0]->x == _x && newPath->firstPoint[0]->y == _y) {
			newPath->numOfPoints--;
			enclosePath(newPath);
		} else {
			appendToPath(newPath, createVectorPoint(_x, _y));
		}
	}

	fclose(externalFile);
	return newPath;
}


// asumsi selalu komplit
VectorPath** createVectorPathFromSVG(char* fileName, int numOfPaths) {
	FILE* externalFile;
	externalFile = fopen(fileName, "r");

	if (externalFile == NULL) {
		printf("[VectorPath.c => createVectorPathFromSVG()] Failed to open external file\n");
		return NULL;
	}


	VectorPath **arrOfPath = malloc(numOfPaths * sizeof(arrOfPath));
	if (!arrOfPath) {
		printf("[VectorPath.c => createVectorPathFromSVG()] Failed to allocate arrOfPath\n");
	}

	char *path = NULL;
	size_t len = 0;
	ssize_t read;
	char *token;
	float _x, _y, sumX, sumY;
	int isFirstPoint = 1;
	int pathCount = 0;

	while ((read = getline(&path, &len, externalFile)) != -1) {
		token = strtok(path, " ");
		while (token != NULL) {
			if (token[0] == ';') {
				// Sudah titik terakhir => tutup polygon
				enclosePath(arrOfPath[pathCount]);
				pathCount++;
				isFirstPoint = 1;
			} else if (sscanf(token, "%f,%f", &_x, &_y) == 2) {
				if (isFirstPoint == 1) {
					// add new path here
					arrOfPath[pathCount] = NULL;
					sumX = _x;
					sumY = _y;
					arrOfPath[pathCount] = createVectorPath(createVectorPoint(sumX, sumY));

					arrOfPath[pathCount]->maxX = sumX;
					arrOfPath[pathCount]->maxY = sumY;
					arrOfPath[pathCount]->minX = sumX;
					arrOfPath[pathCount]->minY = sumY;

					isFirstPoint = 0;
				} else {
					sumX += _x;
					sumY += _y;

					// update max min x, y
					if (arrOfPath[pathCount]->maxX < sumX) {
						arrOfPath[pathCount]->maxX = sumX;
					}
					if (arrOfPath[pathCount]->maxY < sumY) {
						arrOfPath[pathCount]->maxY = sumY;
					}

					if (arrOfPath[pathCount]->minX > sumX) {
						arrOfPath[pathCount]->minX = sumX;
					}
					if (arrOfPath[pathCount]->minY > sumY) {
						arrOfPath[pathCount]->minY = sumY;
					}

					appendToPath(arrOfPath[pathCount], createVectorPoint(sumX, sumY));
				}
				arrOfPath[pathCount]->numOfPoints++;
			}
			token = strtok(NULL, " ");
		}
	}

	fclose(externalFile);
	free(path);
	return arrOfPath;
}

void freeVectorPath(VectorPath* vectorPath) {
	if (vectorPath != NULL) {
		if (vectorPath->lastPoint[0] != NULL) {
			vectorPath->lastPoint[0]->nextPoint[0] = NULL;
			free(vectorPath->lastPoint);
		}
		if (vectorPath->firstPoint[0] != NULL) {
			VectorPoint* currentToDelete = vectorPath->firstPoint[0];
			VectorPoint* nextToDelete = currentToDelete->nextPoint[0];

			do {
				if (VectorPath_debug == 1) {
					printf("freeing this vector: (%.2f, %.2f)\n", currentToDelete->x, currentToDelete->y);
				}
				freeVectorPoint(currentToDelete);
				currentToDelete = nextToDelete;
				if (currentToDelete != NULL) {
					nextToDelete = currentToDelete->nextPoint[0];
				}
			} while (currentToDelete != NULL);
			free(vectorPath->firstPoint);
		}

		free(vectorPath);

		if (VectorPath_debug == 1) {
			printf("successfully freed path\n");
		}
	}
}

void checkForMinMaxUpdate(VectorPath* path) {
	if (path != NULL) {
        if (path->firstPoint[0] != NULL && path->firstPoint[0]->nextPoint[0] != NULL) {
            VectorPoint** currentPoint = path->firstPoint;
            VectorPoint** nextPoint = path->firstPoint[0]->nextPoint;

            path->maxX = currentPoint[0]->x;
			path->maxY = currentPoint[0]->y;
			path->minX = currentPoint[0]->x;
			path->minY = currentPoint[0]->y;

            do {
                if (path->maxX < currentPoint[0]->x) {
					path->maxX = currentPoint[0]->x;
				}
				if (path->maxY < currentPoint[0]->y) {
					path->maxY = currentPoint[0]->y;
				}

				if (path->minX > currentPoint[0]->x) {
					path->minX = currentPoint[0]->x;
				}
				if (path->minY > currentPoint[0]->y) {
					path->minY = currentPoint[0]->y;
				}

                currentPoint = nextPoint;
                if (currentPoint[0] != NULL) {
                    nextPoint = currentPoint[0]->nextPoint;
                }
            } while (currentPoint[0] != NULL && currentPoint[0] != path->firstPoint[0]);
        } else {
            
        }
    } else {
        
    }
}

void appendToPath(VectorPath* vectorPath, VectorPoint* newPoint) {
	if (vectorPath == NULL) {
		printf("[VectorPath.c => appendToPath()] The vector path given has a value of NULL\n");
	} else if (newPoint == NULL) {
		printf("[VectorPath.c => appendToPath()] The point given has a value of NULL\n");
	} else {
		if (vectorPath->firstPoint[0] != NULL) {
			if (isPathMember(vectorPath, newPoint)) {
				printf("[VectorPath.c => appendToPath()] The path already contains the input point. \nUse makePathCyclic() if you want to connect the first and the last point of a path\n");	
			}

			if (vectorPath->lastPoint[0] != NULL) {
				// Check if cyclic
				if (checkIfPathIsClosed(vectorPath)) {
					setPrevPoint(vectorPath->firstPoint[0], newPoint);
				}

				setNextPoint(vectorPath->lastPoint[0], newPoint);
			} else {
				setNextPoint(vectorPath->firstPoint[0], newPoint);
			}

			vectorPath->lastPoint[0] = newPoint;
		} else {
			vectorPath->firstPoint[0] = newPoint;
		}
	}

}

void prependToPath(VectorPath* vectorPath, VectorPoint* newPoint) {
	printf("prependPath is not implemented yet :P\n");
}

void insertToPath(VectorPath* vectorPath, VectorPoint* targetPoint, VectorPoint* newPoint) {
	printf("insertToPath is not implemented yet :P\n");
}

void removeFirstPoint(VectorPath* vectorPath) {
	printf("removeFirstPoint is not implemented yet :P\n");
}

void removeLastPoint(VectorPath* vectorPath) {
	printf("removeLastPoint is not implemented yet :P\n");
}

void removePoint(VectorPath* vectorPath, VectorPoint* targetPoint) {
	printf("removePoint is not implemented yet :P\n");
}

void enclosePath(VectorPath* vectorPath) {
	if (vectorPath == NULL) {
		printf("[VectorPath.c => enclosePath()] The path has a value of NULL\n");
	}

	if (vectorPath->firstPoint[0] != NULL) {
		int count = 0;
		VectorPoint** current = vectorPath->firstPoint;
		VectorPoint** next = vectorPath->firstPoint[0]->nextPoint;
		while (count < 2 && next[0] != NULL && next[0] != vectorPath->firstPoint[0]) {
			count++;
			current = next;
			next = current[0]->nextPoint;
		}

		if (count < 2) {
			printf("[VectorPath.c => enclosePath()] The path must have at least 3 points (was given %d)\n", count);
		} else {
			setNextPoint(vectorPath->lastPoint[0], vectorPath->firstPoint[0]);
		}
	}
}

int checkIfPathIsClosed(VectorPath* vectorPath) {
	if (vectorPath->firstPoint[0] == NULL || vectorPath->lastPoint[0] ==NULL) {
		return 0;
	}
	return (vectorPath->lastPoint[0]->nextPoint[0] == vectorPath->firstPoint[0]);
}

int isPathMember(VectorPath* vectorPath, VectorPoint* wantedPoint) {
	if (vectorPath == NULL) {
		printf("[VectorPath.c => findPathMember()] The vector path given has a value of NULL\n");
		return -1;
	} else if (wantedPoint == NULL) {
		printf("[VectorPath.c => findPathMember()] The point given has a value of NULL\n");
		return -1;
	} else {
		if (vectorPath->firstPoint[0] != NULL) {
			VectorPoint** currentToCheck = vectorPath->firstPoint;
			VectorPoint** nextToCheck = vectorPath->firstPoint[0]->nextPoint;

			do {
				if (currentToCheck[0] == wantedPoint) {
					return 1;
				}
				currentToCheck = nextToCheck;

				if (currentToCheck[0] != NULL) {
					nextToCheck = currentToCheck[0]->nextPoint;
				}
			} while (currentToCheck[0] != NULL && currentToCheck[0] != vectorPath->firstPoint[0]);
		} else {
			printf("[VectorPath.c => findPathMember()] The vector path is empty\n");
			return -1;
		}
	}

	return 0;
}

VectorPoint* findPathMemberByCoordinate(VectorPath* vectorPath, float _x, float _y) {
	if (vectorPath == NULL) {
		printf("[VectorPath.c => findPathMemberByCoordinate()] The vector path given has a value of NULL\n");
	} else {
		if (vectorPath->firstPoint[0] != NULL) {
			VectorPoint** currentToCheck = vectorPath->firstPoint;
			VectorPoint** nextToCheck = vectorPath->firstPoint[0]->nextPoint;

			do {
				if (currentToCheck[0]->x == _x && currentToCheck[0]->y == _y) {
					return currentToCheck[0];
				}
				currentToCheck = nextToCheck;

				if (currentToCheck[0] != NULL) {
					nextToCheck = currentToCheck[0]->nextPoint;
				}
			} while (currentToCheck[0] != NULL && currentToCheck[0] != vectorPath->firstPoint[0]);
		} else {
			printf("[VectorPath.c => findPathMemberByCoordinate()] The vector path is empty\n");
		}
	}

	return NULL;
}

void printPoint(VectorPoint* input) {
	printf("x: %.2f, y: %.2f,  prevPoint: %p, ownAddress: %p,  nextPoint: %p\n", input->x, input->y, input->prevPoint[0], input, input->nextPoint[0]);
}

// use this function pattern to traverse a VectorPath.
void printPath(VectorPath* path) {
	if (path != NULL) {
		printf("First point: %p, Last point: %p\n", path->firstPoint[0], path->lastPoint[0]);
		printf("maxX: %d maxY: %d\n", path->maxX, path->maxY);
		printf("minX: %d minY: %d\n", path->minX, path->minY);
		printf("numOfPoints: %d\n", path->numOfPoints);
		if (path->firstPoint[0] != NULL) {
			VectorPoint** currentToPrint = path->firstPoint;
			VectorPoint** nextToPrint = path->firstPoint[0]->nextPoint;

			do {
				printPoint(currentToPrint[0]);
				currentToPrint = nextToPrint;

				if (currentToPrint[0] != NULL) {
					nextToPrint = currentToPrint[0]->nextPoint;
				}
			} while (currentToPrint[0] != NULL && currentToPrint[0] != path->firstPoint[0]);
		} else {
			printf("Path is empty\n");
		}

		printf("Enclosed: %s", (checkIfPathIsClosed(path) != 0 ? "true" : "false"));
	} else {
		printf("Path is NULL\n");
	}

	printf("\n");
}