#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
typedef struct sBitmap{
	int numOfRows;
	int numOfColumns;
	int** map;
} bitmap;

bitmap* toBitmap();
void printHelp();
int handleThreeArguments(char* args[]);


int main(int argc, char* argv[]) {
	if (argc < 2 || argc > 3) {
		fprintf(stderr, "Invalid number of arguments. \n");
		return 1;
	} else if (argc==2) {
		fprintf(stderr, "Invalid argument");
		return 2;
	} else {
		int returnCode = handleThreeArguments(argv);
		return returnCode;
	}
}
//TODO
bitmap* toBitmap(FILE* source){
	bitmap* newMap = malloc(sizeof(bitmap));
	
	int rows;
	int columns;
	
	int succeeded = fscanf(source, "%i %i", &rows, &columns);
	if (succeeded < 2) {
		return NULL;
	}
		
	newMap->numOfRows = rows;
	newMap->numOfColumns = columns;

	newMap->map = malloc(rows*sizeof(int*));
	for (int i=0;i<rows;i++) {
		newMap->map[i] = malloc(columns*sizeof(int));
		if (newMap->map[i]==NULL) {
			dealloc();
			return NULL;
		}
	}
	for (int i=0;i<rows;i++) {
		for (int j=0;i<columns;j++) {
			scanf("%i", &newMap->map[i][j]);
			if (newMap->map[i][j] < 1) {
				dealloc();
				return NULL;
	return;
}

//TODO
void printHelp() {
	printf("Help");
	return;
}
//TODO
int handleThreeArguments(char* args[]) {
	return args[1][1];
}

void dealloc() {
	for (int i=newMap->rows-1;i>=0;i--)
		free(newMap->map[i]);
	free(newMap);
}
