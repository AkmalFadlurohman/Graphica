#include "VecLetter.h"

int fpeek(FILE * const fp)
{
  const int c = getc(fp);
  return c == EOF ? EOF : ungetc(c, fp);
}

struct Point* pointInit(double _x, double _y) {
	struct Point* point = malloc(sizeof(struct Point));
	if (point == NULL) {
		printf("(pointInit) Failed to allocate memory for a point\n");
		return NULL;
	}

	point->x = _x;
	point->y = _y;

	return point;
}

struct Line* lineInit(double x1, double y1, double x2, double y2) {
	struct Line* line = malloc(sizeof(struct Line));
	if (line == NULL) {
		printf("(lineInit) Failed to allocate memory for a line\n");
		return NULL;
	}

	struct Point* point1 = pointInit(x1, y1);
	if (point1 == NULL) {
		printf("(lineInit) Point 1 has a value of NULL\n");
		free(line);
		return NULL;	
	}

	struct Point* point2 = pointInit(x2, y2);
	if (point2 == NULL) {
		printf("(lineInit) Point 2 has a value of NULL\n");
		free(line);
		return NULL;
	}

	line->points[0] = point1;
	line->points[1] = point2;

	return line;
}

/*struct VecLetter* vecLetterInit(int x, int y, int h, int w, int l, int c, struct Line** arrLines, struct Point** arrCrits) {
	struct VecLetter* letter = malloc(sizeof(struct VecLetter));

	if (letter == NULL) {
		printf("(vecLetterInit) Failed to allocate memory for a letter\n");
		return NULL;
	}

	if (l <= 0) {
		printf("(vecLetterInit) letters must have some lines!\n");
		free(letter);

		return NULL;
	}

	letter->lines = malloc(l * sizeof(struct Line*));
	if (letter->lines == NULL) {
		printf("(vecLetterInit) Failed to allocate memory for line pointers\n");
		free(letter);

		return NULL;
	}

	if (arrLines == NULL) {
		printf("(vecLetterInit) The lines pointers has a value of NULL\n");
		free(letter->lines);
		free(letter);

		return NULL;	
	}
	letter->lines = arrLines;

	if (c > 0) {
		letter->critPoints = malloc(c * sizeof(struct Point*));
		if (letter->critPoints == NULL) {
			printf("(vecLetterInit) Failed to allocate memory for critical point pointers\n");
			free(letter->lines);
			free(letter);

			return NULL;
		}

		if (arrCrits == NULL) {
			printf("Warning: critical points pointer is NULL\n");
		}
		letter->critPoints = arrCrits;
	} else {
		letter->critPoints = NULL;
	}

	letter->posX = x;
	letter->posY = y;
	letter->height = h;
	letter->width = w;
	letter->numOfLines = l;
	letter->numOfCritPoints = c;

	return letter;
}*/

struct VecLetter* vecLetterInit(char name, int x, int y, int h, int w, int l, int c) {
    struct VecLetter* letter = malloc(sizeof(struct VecLetter));
    
    if (letter == NULL) {
        printf("(vecLetterInit) Failed to allocate memory for a letter\n");
        return NULL;
    }
    
    if (l <= 0) {
        printf("(vecLetterInit) letters must have some lines!\n");
        free(letter);
        
        return NULL;
    }
    
    letter->lines = malloc(l * sizeof(struct Line*));
    if (letter->lines == NULL) {
        printf("(vecLetterInit) Failed to allocate memory for line pointers\n");
        free(letter);
        
        return NULL;
    }
    
    if (c > 0) {
        letter->critPoints = malloc(c * sizeof(struct Point*));
        if (letter->critPoints == NULL) {
            printf("(vecLetterInit) Failed to allocate memory for critical point pointers\n");
            free(letter->lines);
            free(letter);
            
            return NULL;
        }
    } else {
        letter->critPoints = NULL;
    }
    
    letter->name = name;
	letter->posX = x;
    letter->posY = y;
    letter->height = h;
    letter->width = w;
    letter->numOfLines = l;
    letter->numOfCritPoints = c;
    
    return letter;
}

void freePoint(struct Point* point){
	if (point != NULL) {
		free(point);
	}
}

void freeLine(struct Line* line) {
	if (line != NULL) {
		for (int i = 0; i < 2; i++) {
			freePoint(line->points[i]);
		}

		free(line);
	}
}

void freeVecLetter(struct VecLetter* letter) {
	if (letter != NULL) {
		if (letter->lines != NULL) {
			for (int i = 0; i < letter->numOfLines; i++) {
				freeLine(letter->lines[i]);
			}
			
			free(letter->lines);
		}

		if (letter->critPoints != NULL) {
			for (int i = 0; i < letter->numOfCritPoints; i++) {
				freePoint(letter->critPoints[i]);
			}

			free(letter->critPoints);
		}

		free(letter);
	}
}
