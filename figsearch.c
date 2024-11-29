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
    hline <file>	finds first longest horizontal line and prints \
its start and end coordinates to stdout\n\
    vline <file>	finds first longest vertical line and prints its \
start and end coordinates to stdout\n\
    square <file>	finds first biggest square outline and prints its \
start and end coordinates to stdout\n\
    test <file>		prints \"Valid\" to stdout if given file containts \
valid bitmap, prints \"Invalid\" to stdout otherwise\n"

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

// structure for pair of coordinates of biggest shape found
typedef struct Sshape
{
	point start;
	point end;
} shape;

// header declarations
bitmap *toBitmap(FILE *source);
int handleThreeArguments(char *args[]);
void handleCorrectArg(char *arg, bitmap *inputMap);
int handleTwoArguments(char *arg);
void dealloc(bitmap *map);
shape findHlineOrVline(bitmap *map, char option);
shape findSquare(bitmap *map);
bool isSquare(int side, bitmap *map, point start);

int main(int argc, char *argv[])
{
	int returnCode;

	// separating based on number of arguments
	if (argc < 2 || argc > 3)
	{
		fprintf(stderr, "Invalid number of arguments. For help, run with argument --help. \n");
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

	// inputSucceeded < 2 means that rows or columns weren't succesfully read from input
	if (inputSucceeded < 2 || rows < 1 || columns < 1)
	{
		newMap->invalidity = 1;
		return newMap;
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

	// allocation of each row
	for (int i = 0; i < rows; i++)
	{
		newMap->image[i] = malloc(columns * sizeof(int));
		if (newMap->image[i] == NULL)
		{
			dealloc(newMap);
			return NULL;
		}
	}

	// reading input values from source file
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < columns; j++)
		{
			inputSucceeded = fscanf(source, "%i", &newMap->image[i][j]);
			// verifying of scaned number, terminating scaning otherwise (if scanned character was not
			// whitespace or 0 or 1)
			if (inputSucceeded != 1 || newMap->image[i][j] > 1 || newMap->image[i][j] < 0)
			{
				newMap->invalidity = 1;
				return newMap;
			}
		}
	}

	newMap->invalidity = 0;
	return newMap;
}

// prepares bitmap and checks if first argument is valid
int handleThreeArguments(char *args[])
{
	// checks if first argument matches with any possible keyword, if not, ends program accordingly
	if (strcmp(args[1], "hline") && strcmp(args[1], "vline") &&
			strcmp(args[1], "square") && strcmp(args[1], "test"))
	{
		fprintf(stderr, "Invalid arguments. For help, run with argument --help. \n");
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

	// checks if allocation succeeded and argument isn't test
	if (inputMap == NULL && strcmp(args[1], "test"))
	{
		fprintf(stderr, "Error allocating memory. ");
		return 3;
	}
	// checks if allocation failed and argument is test or bitmap is invalid
	if (inputMap == NULL || inputMap->invalidity)
	{
		fprintf(stderr, "Invalid");
		dealloc(inputMap);
		return 4;
	}

	// calls specific function if setup was successful
	handleCorrectArg(args[1], inputMap);

	dealloc(inputMap);
	return 0;
}

// calls specific function on bitmap based on argument
void handleCorrectArg(char *arg, bitmap *inputMap)
{
	shape biggest;

	if (!strcmp(arg, "hline"))
	{
		biggest = findHlineOrVline(inputMap, 'h');
	}
	else if (!strcmp(arg, "vline"))
	{
		biggest = findHlineOrVline(inputMap, 'v');
	}
	else if (!strcmp(arg, "square"))
	{
		biggest = findSquare(inputMap);
	}
	else
	{
		printf("Valid");
		return;
	}

	// prints coordinates if given shape was found, prints not found otherwise
	if (biggest.start.x != -1)
		printf("%i %i %i %i\n", biggest.start.x, biggest.start.y, biggest.end.x, biggest.end.y);
	else
		printf("Not found\n");

	return;
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
		fprintf(stderr, "Invalid argument. For help, run with argument --help. \n");
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

// finds longest line in bitmap horizontally (option == 'h') or vertically (option == 'v'),
// returns start and end points of it
shape findHlineOrVline(bitmap *map, char option)
{
	int maxLenght = 0;
	int currentLenght = 0;
	shape longest = {{-1, -1}, {-1, -1}};
	point beginningOfCurrent;
	point currentPoint;

	// setting which way to search the image (horizontally or vertically)
	int *primaryCoord = option == 'v' ? &currentPoint.x : &currentPoint.y;
	int *secondaryCoord = option == 'v' ? &currentPoint.y : &currentPoint.x;
	int *primaryDirectionSize = option == 'v' ? &map->numOfRows : &map->numOfColumns;
	int *secondaryDirectionSize = option == 'v' ? &map->numOfColumns : &map->numOfRows;

	// going through all coordinates and finding longest line from each,
	// evaluating it and skipping to end of it
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
			// if line is longer than previous longest or is same size,
			// but starts earlier (lower row, then lower column)
			if (currentLenght > maxLenght ||
					(currentLenght == maxLenght && beginningOfCurrent.x < longest.start.x))
			{
				maxLenght = currentLenght;
				longest.start = beginningOfCurrent;
				*primaryCoord -= 1; // off by one because while increments it one more time before stopping
				longest.end = currentPoint;
			}
			// stops iterating if it cant find any longer line
			if (maxLenght == *primaryDirectionSize)
			{
				return longest;
			}
			currentLenght = 0;
		}
	}

	return longest;
}

// finds biggest outline of square in bitmap,
// returns start (top left) and end (bottom right) points of if
shape findSquare(bitmap *map)
{
	int maxSide = -1;
	int maxPossibleSide;
	shape biggestSquare = {{-1, -1}, {-1, -1}};
	point currentPoint;

	// loops through whole map until no bigger square can be found
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

				// TODO PREDELAT (od nejmensiho)
				int maxPossibleSideX, maxPossibleSideY;
				maxPossibleSideX = map->numOfRows - currentPoint.x - 1;
				maxPossibleSideY = map->numOfColumns - currentPoint.y - 1;
				maxPossibleSide = maxPossibleSideX < maxPossibleSideY ? maxPossibleSideX : maxPossibleSideY;

				// tries to find biggest square possible
				for (int currentSide = maxPossibleSide; currentSide > maxSide; currentSide--)
				{
					if (isSquare(currentSide, map, currentPoint))
					{
						// overides biggest square if bigger is found
						if (currentSide > maxSide)
						{
							maxSide = currentSide;
							biggestSquare.start = currentPoint;
							biggestSquare.end.x = currentPoint.x + currentSide;
							biggestSquare.end.y = currentPoint.y + currentSide;
						}
						break;
					}
				}
			}
		}
	}

	return biggestSquare;
}

// returns true if square is found starting at point start (square of side lenght side)
bool isSquare(int side, bitmap *map, point start)
{
	point offset = {0, 0};
	// checks down from starting point to side lenght, return false if checked pixel is 0
	for (offset.x = 0; offset.x < side; offset.x++)
	{
		// checks each pixel
		if (map->image[start.x + offset.x][start.y + offset.y] == 0)
			return false;
	}
	// checks pixel right from last checked pixel to side lenght
	for (offset.y = 0; offset.y < side; offset.y++)
	{
		// checks each pixel
		if (map->image[start.x + offset.x][start.y + offset.y] == 0)
			return false;
	}
	// checks pixel up from last checked pixel to offset 0 on x axis
	for (; offset.x > 0; offset.x--)
	{
		// checks each pixel
		if (map->image[start.x + offset.x][start.y + offset.y] == 0)
			return false;
	}
	// checks pixel left from last checked pixel to start point
	for (; offset.y > 0; offset.y--)
	{
		// checks each pixel
		if (map->image[start.x + offset.x][start.y + offset.y] == 0)
			return false;
	}

	// returns true if every pixel was one (black)
	return true;
}
