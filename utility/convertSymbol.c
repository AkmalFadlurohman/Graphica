#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

void main() {
	FILE* reader;

	reader = fopen("planeSym.txt", "r");
	if (reader == NULL) {
		printf("Cannot open file \n");
		fclose(reader);
		return;
	}

	FILE* writer;

	writer = fopen("plane.txt", "w");
	if (writer == NULL) {
		printf("Cannot open file \n");
		fclose(writer);
		return;
	}

	while (!feof(reader)) {
		char c = fgetc(reader);
		switch (c) {
			case '^' :
				fprintf(writer, "(0,0,0,255)"); // transparent
				break;
			case '0' :
				fprintf(writer, "(0,0,0,0)"); // black
				break;
			case 'f' :
				fprintf(writer, "(255,255,255,0)"); // white
				break;
			case 'b' :
				fprintf(writer, "(0,0,255,0)"); // blue
				break;
			case 'r' :
				fprintf(writer, "(255,0,0,0)"); // red
				break;
			case '\n':
				fprintf(writer, "\n");
				break;
		}
	}

	fclose(reader);
	fclose(writer);
}