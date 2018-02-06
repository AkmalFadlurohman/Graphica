#include "../headers/f_image.h"

void main() {
	char* fileName = "../assets/plane.txt";

	struct f_Image* plane = f_loadImage(fileName);

	for (int i = 0; i < plane->width * plane->height; i++) {
		printf("pixel %d: %d\n", i, *(plane->bitmap[i]));
	}
	printf("width: %d\n", plane->width);
	printf("height: %d\n", plane->height);
	printf("posX: %d\n", plane->posX);
	printf("posY: %d\n", plane->posY);

	f_freeImage(plane);
}