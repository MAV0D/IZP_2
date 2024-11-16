/* Program for finding longest lines and biggest squares in bitmap.
 *  Takes hline, vline (horizontal/vertical line), square and test (for checking validity
 *  of input file) as first parameter and file name as second.
 *  Additionally, takes --help as only parameter to print help and end program.
 *  Returns 0 as standard return code, 1 if arguments are not supported, 2 if file can't be opened,
 *  3 if not enough memory can be allocated and 4 if file contains invalid data.
 *  Vojtech Zmatlo (xzmatlv00), 11/2024
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define HELP "Usage: .\\figsearch [option] <input-file>\n\n\
Options:\n\
    --help		prints help message and exits\n\
    hline <file>	finds first longest horizontal line and prints its start and end coordinates to stdout\n\
    vline <file>	finds first longest vertical line and prints its start and end coordinates to stdout\n\
    square <file>	finds first biggest square outline and prints its start and end coordinates to stdout\n\
    test <file>		prints \"Valid\" to stdout if given file containts valid bitmap, prints \"Invalid to stdout otherwise\"\n"

// structure for storing information about image
typedef struct Sbitmap
{
	int numOfRows;
	int numOfColumns;
	int **image;
	bool invalidity;
} bitmap;

// structure for storing coordinates
typedef struct Spoint
{
	int x;
	int y;
} point;

// header declarations
bitmap *toBitmap(FILE *source);
int handleThreeArguments(char *args[]);
int handleCorrestArg(char *arg, bitmap *inputMap);
int handleTwoArguments(char *arg);
void dealloc(bitmap *map);
int findHlineOrVline(bitmap *map, char option);
int findSquare(bitmap *map);
bool isSquare(int side, bitmap *map, point start);

int main(int argc, char *argv[])
{
	int returnCode;

	// separating based on number of arguments
	if (argc < 2 || argc > 3)
	{
		fprintf(stderr, "Invalid number of arguments. \n");
		return 1;
	}
	else if (argc == 2)
	{
		returnCode = handleTwoArguments(argv[1]);
	}
	else
	{
		returnCode = handleThreeArguments(argv);
	}

	return returnCode;
}

// allocates bitmap from source file to structure bitmap, returns NULL in case of any error
bitmap *toBitmap(FILE *source)
{
	// allocation and check of pointer validity
	bitmap *newMap = malloc(sizeof(bitmap));
	if (newMap == NULL)
	{
		return NULL;
	}
	int rows;
	int columns;
	int inputSucceeded; // variable for verification of input

	// scanning and verifying dimensions of bitmap
	inputSucceeded = fscanf(source, "%i %i", &rows, &columns);
	if (inputSucceeded < 2 || rows < 1 || columns < 1)
	{
		newMap->invalidity = 1;
		return NULL;
	}
	newMap->numOfRows = rows;
	newMap->numOfColumns = columns;

	// allocation of space for map
	newMap->image = malloc(rows * sizeof(int *));
	if (newMap->image == NULL)
	{
		dealloc(newMap);
		return NULL;
	}
	for (int i = 0; i < rows; i++)
	{
		newMap->image[i] = malloc(columns * sizeof(int));
		if (newMap->image[i] == NULL)
		{
			dealloc(newMap);
			return NULL;
		}
	}

	// reading input values form source stream
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			inputSucceeded = fscanf(source, "%i", &newMap->image[i][j]);
			// verifying of scaned number and source stream persistence, terminating scaning otherwise
			if (inputSucceeded == -1 || newMap->image[i][j] > 1 || newMap->image[i][j] < 0)
			{
				newMap->invalidity = 1;
				return newMap;
			}
		}
	}

	// checking if file has ended, if not, marking bitmap as invalid
	int temp;
	inputSucceeded = fscanf(source, "%i", &temp);
	if (inputSucceeded != -1)
	{
		newMap->invalidity = 1;
		return newMap;
	}

	newMap->invalidity = 0;
	return newMap;
}

// prepares bitmap and checks if argument is valid
int handleThreeArguments(char *args[])
{
	// checks if first argument matches with any possible keyword, if not, ends program accordingly
	if (strcmp(args[1], "hline") && strcmp(args[1], "vline") && strcmp(args[1], "square") && strcmp(args[1], "test"))
	{
		fprintf(stderr, "Invalid arguments.\n");
		return 1;
	}

	// opens given file, ends program it fails
	FILE *srcFile = fopen(args[2], "r");
	if (srcFile == NULL)
	{
		fprintf(stderr, "Error opening file");
		return 2;
	}

	// makes bitmap from file, closes no longer needed file, ends program if it failes
	bitmap *inputMap = toBitmap(srcFile);

	fclose(srcFile);

	if (inputMap == NULL)
	{
		fprintf(stderr, "Error allocating memory. ");
		dealloc(inputMap);
		return 3;
	}
	if (inputMap->invalidity)
	{
		fprintf(stderr, "Invalid");
		dealloc(inputMap);
		return 4;
	}

	// calls specific function if setup was successful
	int returnCode = handleCorrestArg(args[1], inputMap);

	dealloc(inputMap);
	return returnCode;
}

// calls specific function based on argument
int handleCorrestArg(char *arg, bitmap *inputMap)
{
	int returnCode;

	if (!strcmp(arg, "hline"))
	{
		returnCode = findHlineOrVline(inputMap, 'h');
	}
	else if (!strcmp(arg, "vline"))
	{
		returnCode = findHlineOrVline(inputMap, 'v');
	}
	else if (!strcmp(arg, "square"))
	{
		returnCode = findSquare(inputMap);
	}
	else
	{
		printf("Valid");
		returnCode = 0;
	}

	return returnCode;
}

// handles calls of program with two arguments (prints help for --help)
int handleTwoArguments(char *arg)
{
	if (!strcmp(arg, "--help"))
	{
		printf(HELP);
		return 0;
	}
	else
	{
		fprintf(stderr, "Invalid argument.\n");
		return 1;
	}
}

// deallocates all allocated memory of bitmap
void dealloc(bitmap *map)
{
	for (int i = map->numOfRows - 1; i >= 0; i--)
		free(map->image[i]);
	free(map->image);
	free(map);
}

// finds longest line horizontally (option == 'h') or vertically (option == 'v'), prints start and end points of it
int findHlineOrVline(bitmap *map, char option)
{
	int maxLenght = 1;
	int currentLenght = 0;
	point beginningOfLongest;
	point endOfLongest;
	point beginningOfCurrent;
	point currentPoint;

	// setting which way to search the image (horizontally or vertically)
	int *primaryCoord = option == 'v' ? &currentPoint.x : &currentPoint.y;
	int *secondaryCoord = option == 'v' ? &currentPoint.y : &currentPoint.x;
	int *primaryDirectionSize = option == 'v' ? &map->numOfRows : &map->numOfColumns;
	int *secondaryDirectionSize = option == 'v' ? &map->numOfColumns : &map->numOfRows;

	// going through all coordinates and finding longest line from each, evaluating it and skipping to end of it
	for ((*secondaryCoord) = 0; (*secondaryCoord) < *secondaryDirectionSize; (*secondaryCoord)++)
	{
		for ((*primaryCoord) = 0; (*primaryCoord) < *primaryDirectionSize; (*primaryCoord)++)
		{
			beginningOfCurrent = currentPoint;
			// continues in line while pixels equal 1 and while in bounds of map
			while (*primaryCoord < *primaryDirectionSize && map->image[currentPoint.x][currentPoint.y])
			{
				currentLenght++;
				(*primaryCoord)++;
			}
			// if line is longer than previous longest or is same size, but starts earlier (lower row, than lower column)
			if (currentLenght > maxLenght ||
					(currentLenght == maxLenght && beginningOfCurrent.x < beginningOfLongest.x))
			{
				maxLenght = currentLenght;
				beginningOfLongest = beginningOfCurrent;
				*primaryCoord -= 1; // off by one because while increments it one more time before stopping
				endOfLongest = currentPoint;
			}
			currentLenght = 0;
		}
	}

	// if no line is found (no pixels or only points)
	if (maxLenght < 2)
	{
		printf("Not found");
		return 0;
	}

	printf("%i %i %i %i\n", beginningOfLongest.x, beginningOfLongest.y, endOfLongest.x, endOfLongest.y);
	return 0;
}

// finds biggest outline of square regardless of the inside, prints start (top left) and end (bottom right) points of if
int findSquare(bitmap *map)
{
	int maxSide = 1;
	point beginningOfBiggest;
	point endOfBiggest;
	point currentPoint;

	// loops through whole map, stops when no bigger square can be found
	for (currentPoint.x = 0; currentPoint.x < map->numOfRows - maxSide; currentPoint.x++)
	{
		for (currentPoint.y = 0; currentPoint.y < map->numOfColumns - maxSide; currentPoint.y++)
		{
			// tries to find biggest square from pixel (going bottom right) if current pixel is '1'
			// on failure tries smaller squares, on success goes to next pixel
			if (map->image[currentPoint.x][currentPoint.y])
			{
				// calcuates maximum side lenght of square by substracting current coords from size of map
				// and than using the lower one
				point maxPossibleSides;
				maxPossibleSides.x = map->numOfRows - currentPoint.x - 1;
				maxPossibleSides.y = map->numOfColumns - currentPoint.y - 1;
				int maxPossibleSide = maxPossibleSides.x < maxPossibleSides.y ? maxPossibleSides.x : maxPossibleSides.y;

				// tries to find biggest square possible
				for (int currentSide = maxPossibleSide; currentSide > maxSide; currentSide--)
				{
					if (isSquare(currentSide, map, currentPoint))
					{
						// overides biggest square if bigger is found
						if (currentSide > maxSide)
						{
							maxSide = currentSide;
							beginningOfBiggest = currentPoint;
							endOfBiggest.x = currentPoint.x + currentSide;
							endOfBiggest.y = currentPoint.y + currentSide;
						}
						break;
					}
				}
			}
		}
	}

	// if no line is found (no pixels or only points)
	if (maxSide < 2)
		printf("Not found");
	else
		printf("%i %i %i %i\n", beginningOfBiggest.x, beginningOfBiggest.y, endOfBiggest.x, endOfBiggest.y);

	return 0;
}

// returns true if square is found starting at point start of side lenght side
bool isSquare(int side, bitmap *map, point start)
{
	point offset = {0, 0};
	// checks down from starting point to side lenght, return false if checked pixel is 0
	for (offset.x = 0; offset.x < side; offset.x++)
	{
		if (start.x + offset.x < map->numOfRows && map->image[start.x + offset.x][start.y + offset.y] == 0)
			return false;
	}
	// checks pixel right from last checked pixel to side lenght
	for (offset.y = 0; offset.y < side; offset.y++)
	{
		if (start.y + offset.y < map->numOfColumns && map->image[start.x + offset.x][start.y + offset.y] == 0)
			return false;
	}
	// checks pixel up from last checked pixel to offset 0 on x axis
	for (; offset.x > 0; offset.x--)
	{
		if (map->image[start.x + offset.x][start.y + offset.y] == 0)
			return false;
	}
	// checks pixel left from last checked pixel to start point
	for (; offset.y > 0; offset.y--)
	{
		if (map->image[start.x + offset.x][start.y + offset.y] == 0)
			return false;
	}

	// returns true if every pixel was one
	return true;
}
