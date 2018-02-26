#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
	FILE* fp;
	fp = fopen("path1.txt", "r");

	if (fp == NULL) {
		printf("[VectorPath.c => createVectorPathFromFile()] Failed to open external file\n");
		return 0;
	}

	char *path = NULL;
	size_t len = 0;
	ssize_t read;
	char *token;
	float x, y;

	while ((read = getline(&path, &len, fp)) != -1) {
		token = strtok(path, " ");
		while (token != NULL) {
			if (token[0] == ';') {
				printf("%c => end\n", token[0]);
			} else if (sscanf(token, "%f,%f", &x, &y) == 2) {
				printf("%s => add point\n", token);
			}
			token = strtok(NULL, " ");
		}
	}

	fclose(fp);
	free(path);

	return 0;
}