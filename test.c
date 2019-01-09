/*
Copyright (c) 2018 Felipe Ferreira da Silva

This software is provided 'as-is', without any express or implied warranty. In
no event will the authors be held liable for any damages arising from the use of
this software.

Permission is granted to anyone to use this software for any purpose, including
commercial applications, and to alter it and redistribute it freely, subject to
the following restrictions:

  1. The origin of this software must not be misrepresented; you must not claim
     that you wrote the original software. If you use this software in a
     product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.

Compiling with GCC:
  gcc test.c uastar.c -o test
*/

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include "uastar.h"

double passable_chance;

static uint8_t fill_cb(struct path_finder *path_finder, int32_t col, int32_t row)
{
	uint8_t is_passable;
	is_passable = 0;
	/* Fill the map randomly with passable cells */
	if ((double)rand() / (double)RAND_MAX <= passable_chance) {
		is_passable = 1;
	}
	return is_passable;
}

void print_map(struct path_finder *path_finder)
{
	int32_t row = 0;
	int32_t col = 0;
	printf("Map:\n");
	col = 0;
	printf(" =");
	while (col < path_finder->cols - 1) {
		printf("==");
		col++;
	}
	printf("== \n");
	while (row < path_finder->rows) {
		col = 0;
		printf("|");
		while (col < path_finder->cols) {
			if (path_finder_is_start(path_finder, col, row) == 1) {
				if (path_finder_is_passable(path_finder, col, row) == 1) {
					printf(" S");
				} else {
					printf(" s");
				}
			} else if (path_finder_is_end(path_finder, col, row) == 1) {
				if (path_finder_is_passable(path_finder, col, row) == 1) {
					printf(" E");
				} else {
					printf(" e");
				}
			} else if (path_finder_is_path(path_finder, col, row) == 1) {
				printf(" *");
			} else if (!path_finder_is_passable(path_finder, col, row) == 1) {
				printf(" O");
			} else {
				printf("  ");
			}
			col++;
		}
		printf(" |\n");
		row++;
	}
	col = 0;
	printf(" =");
	while (col < path_finder->cols - 1) {
		printf("==");
		col++;
	}
	printf("== \n");
	if (path_finder->has_path) {
		printf("A path was found!\n\n");
	} else {
		printf("No path was found!\n\n");
	}
}

int main(int argc, char **args)
{
	int32_t width;
	int32_t height;
	int32_t s_col;
	int32_t s_row;
	int32_t e_col;
	int32_t e_row;
	struct path_finder path_finder;
	if (argc != 9) {
		puts("Usage: test [passable chance 0-100] [random seed integer] [start column] [start row] [end column] [end row] [width <= 64] [height <= 64]");
		goto done;
	}
	passable_chance = (double)atoi(args[1]) / 100.0;
	srand(atoi(args[2]));
	s_col = atoi(args[3]);
	s_row = atoi(args[4]);
	e_col = atoi(args[5]);
	e_row = atoi(args[6]);
	width = atoi(args[7]);
	height = atoi(args[8]);
	if (width < 1 || width > 64 || height < 1 || height > 64) {
		puts("Invalid width or height.");
		goto done;
	}
	if (s_col < 0 || s_col > width - 1
	|| e_col < 0 || e_col > width - 1
	|| s_row < 0 || s_row > height - 1
	|| e_row < 0 || e_row > height - 1) {
		puts("Invalid coordinates of start or end.");
		goto done;
	}
	init_path_finder(&path_finder);
	path_finder.cols = width;
	path_finder.rows = height;
	path_finder.fill_func = fill_cb;
	path_finder.score_func = NULL;
	path_finder_fill(&path_finder);
	path_finder_set_start(&path_finder, s_col, s_row);
	path_finder_set_end(&path_finder, e_col, e_row);
	path_finder_find(&path_finder, NULL);
	printf("passable chance: %0.1f\n", passable_chance * 100.0);
	printf("            Start: S (or s if fall in a wall)\n");
	printf("              End: E (or e if fall in a wall)\n");
	printf("             Path: *\n");
	printf("       unpassable: O\n");
	print_map(&path_finder);
done:
	return EXIT_SUCCESS;
}
