#include "../headers/VectorPath.h"

void newLine() {
	printf("\n");
}

void printPoint(VectorPoint* input) {
	printf("x: %.2f, y: %.2f,  prevPoint: %p, ownAddress: %p,  nextPoint: %p\n", input->x, input->y, input->prevPoint[0], input, input->nextPoint[0]);
}

// use this function pattern to traverse a VectorPath.
void printPath(VectorPath* path) {
	if (path != NULL) {
		printf("First point: %p, Last point: %p\n", path->firstPoint[0], path->lastPoint[0]);
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

	newLine();
}

void testPointLifeCycle() {
	printf("=========================================\n");
	printf("testPointLifeCycle (using setNextPoint)\n");

	VectorPoint* A = createVectorPoint(0,0);
	VectorPoint* B = createVectorPoint(5, 5);
	VectorPoint* C = createVectorPoint(0,10);
	VectorPoint* D = createVectorPoint(-5,5);

	setNextPoint(A, B);
	setNextPoint(B, C);
	setNextPoint(C, D);
	setNextPoint(D, A);

	printf("Points: \n");
	printPoint(A);
	printPoint(B);
	printPoint(C);
	printPoint(D);
	newLine();

	freeVectorPoint(A);
	freeVectorPoint(B);
	freeVectorPoint(C);
	freeVectorPoint(D);
	printf("%p\n", A);
	newLine();
}

void testPathLifeCycle() {
	printf("=========================================\n");
	printf("testPathLifeCycle (using setPrevPoint, enclosed)\n");
	VectorPoint* A = createVectorPoint(0,0);
	VectorPoint* B = createVectorPoint(5, 5);
	VectorPoint* C = createVectorPoint(0,10);
	VectorPoint* D = createVectorPoint(-5,5);

	setPrevPoint(A,D);
	setPrevPoint(D,C);
	setPrevPoint(C,B);
	setPrevPoint(B,A);

	printf("Points: \n");
	printPoint(A);
	printPoint(B);
	printPoint(C);
	printPoint(D);
	newLine();

	VectorPath* thePath = createVectorPath(A);
	
	printf("Path content: \n");
	printPath(thePath);
	newLine();

	freeVectorPath(thePath);
	newLine();
}

void testPathLifeCycle_2() {
	printf("=========================================\n");
	printf("testPathLifeCycle_2 (using setPrevPoint, not enclosed)\n");
	VectorPoint* A = createVectorPoint(0,0);
	VectorPoint* B = createVectorPoint(5, 5);

	setPrevPoint(B,A);

	printf("Points: \n");
	printPoint(A);
	printPoint(B);
	newLine();

	VectorPath* thePath = createVectorPath(A);
	
	printf("Path content: \n");
	printPath(thePath);
	newLine();

	freeVectorPath(thePath);
	newLine();
}

void testAppendToPath() {
	printf("=========================================\n");
	printf("testAppendToPath (enclosed)\n");
	VectorPoint* A = createVectorPoint(0,0);
	VectorPoint* B = createVectorPoint(5, 5);
	VectorPoint* C = createVectorPoint(0,10);
	VectorPoint* D = createVectorPoint(-5,5);


	VectorPoint* E = createVectorPoint(-10,5);
	VectorPoint* F = createVectorPoint(-5,10);
	VectorPoint* G = createVectorPoint(10,1);

	setPrevPoint(A,D);
	setPrevPoint(D,C);
	setPrevPoint(C,B);
	setPrevPoint(B,A);

	printf("Points: \n");
	printPoint(A);
	printPoint(B);
	printPoint(C);
	printPoint(D);
	printPoint(E);
	printPoint(F);
	printPoint(G);
	newLine();

	VectorPath* thePath = createVectorPath(A);
	
	printf("Original path: \n");
	printPath(thePath);
	newLine();

	printf("adding E: \n");
	appendToPath(thePath, E);
	printPath(thePath);
	newLine();

	printf("adding F: \n");
	appendToPath(thePath, F);
	printPath(thePath);
	newLine();

	printf("adding G: \n");
	appendToPath(thePath, G);
	printPath(thePath);
	newLine();

	freeVectorPath(thePath);
	newLine();
}

void testAppendToPath_2() {
	printf("=========================================\n");
	printf("testAppendToPath (not enclosed)\n");
	VectorPoint* A = createVectorPoint(0,0);
	VectorPoint* B = createVectorPoint(5, 5);
	VectorPoint* C = createVectorPoint(0,10);
	VectorPoint* D = createVectorPoint(-5,5);


	VectorPoint* E = createVectorPoint(-10,5);
	VectorPoint* F = createVectorPoint(-5,10);
	VectorPoint* G = createVectorPoint(10,1);

	setPrevPoint(D,C);
	setPrevPoint(C,B);
	setPrevPoint(B,A);

	printf("Points: \n");
	printPoint(A);
	printPoint(B);
	printPoint(C);
	printPoint(D);
	printPoint(E);
	printPoint(F);
	printPoint(G);
	newLine();

	VectorPath* thePath = createVectorPath(A);
	
	printf("Original path: \n");
	printPath(thePath);
	newLine();

	printf("adding E: \n");
	appendToPath(thePath, E);
	printPath(thePath);
	newLine();

	printf("adding F: \n");
	appendToPath(thePath, F);
	printPath(thePath);
	newLine();

	printf("adding G: \n");
	appendToPath(thePath, G);
	printPath(thePath);
	newLine();

	freeVectorPath(thePath);
	newLine();
}

void testAppendToPath_3() {
	printf("=========================================\n");
	printf("testAppendToPath (starting from empty path)\n");
	VectorPoint* A = createVectorPoint(0,0);
	VectorPoint* B = createVectorPoint(5, 5);
	VectorPoint* C = createVectorPoint(0,10);
	VectorPoint* D = createVectorPoint(-5,5);

	printf("Points: \n");
	printPoint(A);
	printPoint(B);
	printPoint(C);
	printPoint(D);
	newLine();

	VectorPath* thePath = createVectorPath(NULL);
	
	printf("Original path: \n");
	printPath(thePath);
	newLine();

	printf("adding A: \n");
	appendToPath(thePath, A);
	printPath(thePath);
	newLine();

	printf("adding B: \n");
	appendToPath(thePath, B);
	printPath(thePath);
	newLine();

	printf("adding C: \n");
	appendToPath(thePath, C);
	printPath(thePath);
	newLine();

	printf("adding D: \n");
	appendToPath(thePath, D);
	printPath(thePath);
	newLine();

	freeVectorPath(thePath);
	newLine();
}

void tesFindMemberByCoordinate() {
	printf("=========================================\n");
	printf("tesFindMemberByCoordinate\n");
	VectorPoint* A = createVectorPoint(0,0);
	VectorPoint* B = createVectorPoint(5, 5);
	VectorPoint* C = createVectorPoint(0,10);
	VectorPoint* D = createVectorPoint(-5,5);

	printf("Points: \n");
	printPoint(A);
	printPoint(B);
	printPoint(C);
	printPoint(D);
	newLine();

	VectorPath* thePath = createVectorPath(NULL);
	appendToPath(thePath, A);
	appendToPath(thePath, B);
	appendToPath(thePath, C);
	appendToPath(thePath, D);

	printf("(0,0) => %p\n", findPathMemberByCoordinate(thePath, 0, 0));
	printf("(5,5) => %p\n", findPathMemberByCoordinate(thePath, 5, 5));
	printf("(0,10) => %p\n", findPathMemberByCoordinate(thePath, 0, 10));
	printf("(-5,5) => %p\n", findPathMemberByCoordinate(thePath, -5, 5));
	printf("(15,7) => %p\n", findPathMemberByCoordinate(thePath, 15, 7));
	printf("(23,0) => %p\n", findPathMemberByCoordinate(thePath, 23, 0));
	newLine();
	freeVectorPath(thePath);
	newLine();
}

void testLoadExternalFile() {
	printf("=========================================\n");
	printf("testLoadExternalFile (right_triangle.txt, 3 points, enclosed = true)\n");
	VectorPath* thePath = createVectorPathFromFile("right_triangle.txt");
	printPath(thePath);
	freeVectorPath(thePath);
	newLine();
}

void testLoadExternalFile_2() {
	printf("=========================================\n");
	printf("testLoadExternalFile (huruf_s.txt, 6 points, enclosed = false)\n");
	VectorPath* thePath = createVectorPathFromFile("huruf_s.txt");
	printPath(thePath);
	freeVectorPath(thePath);
	newLine();
}

int main() {
	// testPointLifeCycle();
	// testPathLifeCycle();
	// tesFindMemberByCoordinate();
	testLoadExternalFile();
	testLoadExternalFile_2();
	// testPathLifeCycle_2();
	// testAppendToPath();
	// testAppendToPath_2();
	// testAppendToPath_3();

	printf("All test succeeded\n");
	newLine();
}