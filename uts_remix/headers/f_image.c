#include "f_image.h"

unsigned int f_rgbaToInt(int r, int g, int b, int a) {
    return a << 24 | r << 16 | g << 8 | b;
}

int f_fpeek(FILE * const fp)
{
  const int c = getc(fp);
  return c == EOF ? EOF : ungetc(c, fp);
}

struct f_Pixel* f_initPixel(int r, int g, int b, int a) {
	struct f_Pixel* pixel = malloc(sizeof(struct f_Pixel));
	if (pixel == NULL) {
		printf("(f_initPixel) Failed to allocate memory for a pixel\n");
		return NULL;
	}

	pixel->color = f_rgbaToInt(r, g, b, a);

	// for testing in windows
	//pixel->color = r + g + b + a;

	return pixel;
} 

void f_modPixel(struct f_Pixel* pixel, int r, int g, int b, int a) {
	pixel->color = f_rgbaToInt(r, g, b, a);

	// for testing in windows
	// pixel->color = r + g + b + a;
}

void f_freePixel(struct f_Pixel* pixel) {
	if (pixel != NULL) {
		free(pixel);
	}
}


struct f_Image* f_initImage(size_t width,size_t height) {
	struct f_Image* imagePtr = malloc(sizeof (struct f_Image));

	if (imagePtr == NULL) {
		printf("(f_initImage) Failed to allocate memory for image!\n");
		return NULL;
	}

	imagePtr->bitmap = malloc(height * width * sizeof(struct f_Pixel*));
	if (imagePtr->bitmap == NULL) {
		printf("(f_initImage) Failed to allocate memory for bitmap!\n");
		free(imagePtr);
		return NULL;
	}

	for (int i = 0; i < height * width; i++) {
		if (i % 2 == 0) {
			imagePtr->bitmap[i] = f_initPixel(255, 182, 105, 0); // pink
		} else {
			imagePtr->bitmap[i] = f_initPixel(125, 125, 125, 0); // grey
		}

		if (imagePtr->bitmap[i] == NULL) {
			printf("(f_initImage) stack trace -> f_initPixel\n");
			for (int m = 0; m <= i; m++) {
				f_freePixel(imagePtr->bitmap[m]);
			}

			free(imagePtr->bitmap);
			free(imagePtr);
			return NULL;
		}
	}

	imagePtr->width = width;
	imagePtr->height = height;
	imagePtr->posX = 0;
	imagePtr-> posY = 0;

	return imagePtr;
}

void f_freeImage(struct f_Image* image) {
	if (image != NULL) {
		for (int i = 0; i < (image->height) * (image->width); i++) {
			f_freePixel(image->bitmap[i]);
		}

		free(image->bitmap);
		free(image);
	}
}

struct f_Image* f_loadImage(char* fileName) {
	struct f_Image* imagePtr = NULL;
	int imgHeight = 0;
	int imgWidth = 0;
	int ret = 0;
	FILE *fptr;

	fptr = fopen(fileName, "r");
	if (fptr == NULL) {
		printf("Cannot open file \n");
		fclose(fptr);
		return (NULL);
	}

	if (fscanf(fptr, "height=%d;width=%d;\n", &imgHeight, &imgWidth) != 2) {
		printf("Failed to read height and/or width!\n");
		fclose(fptr);
		return NULL;
	} else {
		printf("Height = %d | width = %d\n", imgHeight, imgWidth);
	}

	imagePtr = f_initImage(imgWidth, imgHeight);
	if (imagePtr == NULL) {
		printf("(f_loadImage) stack trace -> f_initImage\n");
		fclose(fptr);
		return NULL;
	}

	int i = 0;
	int area = 0;
	while (!feof(fptr) && (i < (imagePtr->height) * (imagePtr->width))) {
		int r, g, b, a;
		if (i % 2 == 0) { // pink
			r = 255;
			g = 182;
			b = 105;
			a = 0;
		} else { //grey
			r = 125;
			g = 125;
			b = 125;
			a = 0;
		}

		if ((fscanf(fptr, "(%d,%d,%d,%d)", &r, &g, &b, &a)) == 4) {
			area++;
		}

		if (f_fpeek(fptr) == '\n') {
			fgetc(fptr);
		}

		f_modPixel(imagePtr->bitmap[i], r, g, b, a);
		i++;
	}

	printf("Pixels read = %d\nExpected amount = %d\n", area, (imgHeight * imgWidth));

	fclose(fptr);
	return imagePtr;
};