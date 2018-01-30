#include "f_VecLetter.h"

int fpeek(FILE * const fp)
{
  const int c = getc(fp);
  return c == EOF ? EOF : ungetc(c, fp);
}

struct f_Point* f_pointInit(double _x, double _y) {
	struct f_Point* point = malloc(sizeof(struct f_Point));	
	if (point == NULL) {
		printf("(f_pointInit) Failed to allocate memory for a point\n");
		return NULL;
	}

	point->x = _x;
	point->y = _y;

	return point;
}

struct f_Line* f_lineInit(double x1, double y1, double x2, double y2) {
	struct f_Line* line = malloc(sizeof(struct f_Line));
	if (line == NULL) {
		printf("(f_lineInit) Failed to allocate memory for a line\n");
		return NULL;
	}

	struct f_Point* point1 = f_pointInit(x1, y1);
	if (point1 == NULL) {
		printf("(f_lineInit) Point 1 has a value of NULL\n");
		free(line);
		return NULL;	
	}

	struct f_Point* point2 = f_pointInit(x2, y2);
	if (point2 == NULL) {
		printf("(f_lineInit) Point 2 has a value of NULL\n");
		free(line);
		return NULL;
	}

	line->points[0] = point1;
	line->points[1] = point2;

	return line;
}

struct f_VecLetter* f_vecLetterInit(int x, int y, int h, int w, int l, int c, struct f_Line** arrLines, struct f_Point** arrCrits) {
	struct f_VecLetter* letter = malloc(sizeof(struct f_VecLetter));

	if (letter == NULL) {
		printf("(f_vecLetterInit) Failed to allocate memory for a letter\n");
		return NULL;
	}

	if (l <= 0) {
		printf("(f_vecLetterInit) letters must have some lines!\n");
		free(letter);

		return NULL;
	}

	letter->lines = malloc(l * sizeof(struct f_Line*));
	if (letter->lines == NULL) {
		printf("(f_vecLetterInit) Failed to allocate memory for line pointers\n");
		free(letter);

		return NULL;
	}

	if (arrLines == NULL) {
		printf("(f_vecLetterInit) The lines pointers has a value of NULL\n");
		free(letter->lines);
		free(letter);

		return NULL;	
	}
	letter->lines = arrLines;

	if (c > 0) {
		letter->critPoints = malloc(c * sizeof(struct f_Point*));
		if (letter->critPoints == NULL) {
			printf("(f_vecLetterInit) Failed to allocate memory for critical point pointers\n");
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
}

void f_freePoint(struct f_Point* point){
	if (point != NULL) {
		free(point);
	}
}

void f_freeLine(struct f_Line* line) {
	if (line != NULL) {
		for (int i = 0; i < 2; i++) {
			f_freePoint(line->points[i]);
		}

		free(line);
	}
}

void f_freeVecLetter(struct f_VecLetter* letter) {
	if (letter != NULL) {
		if (letter->lines != NULL) {
			for (int i = 0; i < letter->numOfLines; i++) {
				f_freeLine(letter->lines[i]);
			}
			
			free(letter->lines);
		}

		if (letter->critPoints != NULL) {
			for (int i = 0; i < letter->numOfCritPoints; i++) {
				f_freePoint(letter->critPoints[i]);
			}

			free(letter->critPoints);
		}

		free(letter);
	}
}


// TODO
struct f_VecLetter** f_loadLetters(char* fileName) {
	FILE *fptr;
	int numOfLetters = 0;

	fptr = fopen(fileName, "r");
	if (fptr == NULL) {
		printf("Cannot open file \n");
		fclose(fptr);
		return (NULL);
	}

	return NULL;
}