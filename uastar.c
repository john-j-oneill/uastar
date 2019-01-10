/*
Copyright (C) 2017 Felipe Ferreira da Silva

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
*/

#include <stddef.h>
#include <stdio.h>
#include "uastar.h"

static int32_t path_finder_heuristic(struct path_finder *path_finder, int32_t tile)
{
	int32_t tile_y;
	int32_t tile_x;
	int32_t end_y;
	int32_t end_x;
	int32_t dx;
	int32_t dy;
	tile_y = tile / path_finder->cols;
	tile_x = tile - (tile_y * path_finder->cols);
	end_y = path_finder->end / path_finder->cols;
	end_x = path_finder->end - (end_y * path_finder->cols);
	dx = 0;
	dy = 0;
	if (tile_x > end_x) {
		dx = tile_x - end_x;
	} else {
		dx = end_x - tile_x;
	}
	if (tile_y > end_y) {
		dy = tile_y - end_y;
	} else {
		dy = end_y - tile_y;
	}
	return dx + dy;
}

static BOOLEAN path_finder_open_set_is_empty(struct path_finder *path_finder)
{
	BOOLEAN empty;
	INDEX_TYPE i;
	empty = 1;
	i = 0;
	while (i < path_finder->cols * path_finder->rows && empty == 1) {
		if (path_finder->nodes[i].open_set == TRUE) {
			empty = 0;
		}
		i = i + 1;
	}
	return empty;
}

static INDEX_TYPE path_finder_lowest_in_open_set(struct path_finder *path_finder)
{
	SCORE_TYPE lowest_f;
	INDEX_TYPE current_lowest;
	INDEX_TYPE i;
	lowest_f = SCORE_MAX;
	current_lowest = 0;
	i = 0;
	while (i < path_finder->cols * path_finder->rows) {
		if (path_finder->nodes[i].open_set == TRUE) {
			if (path_finder->nodes[i].f_score < lowest_f) {
				lowest_f = path_finder->nodes[i].f_score;
				current_lowest = i;
			}
		}
		i = i + 1;
	}
	return current_lowest;
}

static void path_finder_reconstruct_path(struct path_finder *path_finder)
{
	INDEX_TYPE i;
	i = path_finder->end;
	while (i != path_finder->start) {
		if (path_finder->nodes[i].parents != path_finder->start) {
			path_finder->nodes[path_finder->nodes[i].parents].path = TRUE;
		}
		i = path_finder->nodes[i].parents;
	}
}

void path_finder_fill(struct path_finder *path_finder)
{
	INDEX_TYPE row;
	row = 0;
	while (row < path_finder->rows) {
		INDEX_TYPE col;
		col = 0;
		while (col < path_finder->cols) {
			path_finder->nodes[row * path_finder->cols + col].passables = path_finder->fill_func(path_finder, col, row);
			col = col + 1;
		}
		row = row + 1;
	}
}

void path_finder_find(struct path_finder *path_finder, void *data)
{
	BOOLEAN run;
	int32_t neighbors[4];
	INDEX_TYPE current;
	current = 0;
	path_finder->nodes[path_finder->start].open_set = TRUE;
	neighbors[0] = 0;
	neighbors[1] = 0;
	neighbors[2] = 0;
	neighbors[3] = 0;
	run = TRUE;
	while (run == TRUE) {
		current = path_finder_lowest_in_open_set(path_finder);
		if (current == path_finder->end) {
			path_finder_reconstruct_path(path_finder);
			run = FALSE;
			path_finder->has_path = TRUE;
		} else if (path_finder_open_set_is_empty(path_finder) == TRUE) {
			run = FALSE;
			path_finder->has_path = FALSE;
		} else {
			int32_t j;
			SCORE_TYPE tmp_g_score;
			path_finder->nodes[current].open_set = FALSE;
			path_finder->nodes[current].closed_set = TRUE;
			/* Top */
			neighbors[0] = current - path_finder->cols;
			/* Right */
			if ((current + 1) % path_finder->cols == 0) {
				neighbors[1] = -1;
			} else {
				neighbors[1] = current + 1;
			}
			/* Bottom */
			neighbors[2] = current + path_finder->cols;
			/* Left */
			if (current % path_finder->cols == 0) {
				neighbors[3] = -1;
			} else {
				neighbors[3] = current - 1;
			}
			/* Neighbors */
			tmp_g_score = 0;
			j = 0;
			while (j < 4) {
				INDEX_TYPE n;
				n = neighbors[j];
				if (n > -1 && n < path_finder->rows * path_finder->cols && path_finder->nodes[n].closed_set == 0) {
					if (path_finder->nodes[n].passables == FALSE) {
						path_finder->nodes[n].closed_set = TRUE;
					} else {
						tmp_g_score = path_finder->nodes[current].g_score + 1;
						if (path_finder->nodes[n].open_set == FALSE || tmp_g_score < path_finder->nodes[n].g_score) {
							path_finder->nodes[n].parents = current;
							path_finder->nodes[n].g_score = tmp_g_score;
							path_finder->nodes[n].f_score = path_finder->nodes[n].g_score + path_finder_heuristic(path_finder, n);
							if (path_finder->score_func != NULL) {
								path_finder->nodes[n].f_score = path_finder->nodes[n].f_score + path_finder->score_func(path_finder, n / path_finder->cols, n % path_finder->cols, data);
							}
							path_finder->nodes[n].open_set = TRUE;
						}
					}
				}
				j = j + 1;
			}
		}
	}
}

SCORE_TYPE path_finder_get_heuristic_score(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row)
{
	return path_finder->nodes[row * path_finder->cols + col].f_score;
}

BOOLEAN path_finder_is_passable(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row)
{
	BOOLEAN result;
	result = FALSE;
	if (path_finder->nodes[row * path_finder->cols + col].passables == TRUE) {
		result = TRUE;
	}
	return result;
}

BOOLEAN path_finder_is_path(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row)
{
	BOOLEAN result;
	result = FALSE;
	if (path_finder->nodes[row * path_finder->cols + col].path == TRUE) {
		result = TRUE;
	}
	return result;
}

BOOLEAN path_finder_is_start(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row)
{
	BOOLEAN result;
	result = FALSE;
	if (row * path_finder->cols + col == path_finder->start) {
		result = TRUE;
	}
	return result;
}

BOOLEAN path_finder_is_end(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row)
{
	BOOLEAN result;
	result = FALSE;
	if (row * path_finder->cols + col == path_finder->end) {
		result = TRUE;
	}
	return result;
}

void path_finder_set_path(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row, BOOLEAN path)
{
	if (col >= 0 && col < path_finder->cols && row >= 0 && row < path_finder->rows) {
		path_finder->nodes[path_finder->start].path = path;
	}
}

void path_finder_set_start(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row)
{
	if (col >= 0 && col < path_finder->cols && row >= 0 && row < path_finder->rows) {
		path_finder->start = row * path_finder->cols + col;
	}
}

void path_finder_set_end(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row)
{
	if (col >= 0 && col < path_finder->cols && row >= 0 && row < path_finder->rows) {
		path_finder->end = row * path_finder->cols + col;
	}
}

void path_finder_clear_path(struct path_finder *path_finder)
{
	INDEX_TYPE i;
	i = 0;
	while (i < PATH_FINDER_MAX_CELLS) {
		path_finder->nodes[i].open_set = FALSE;
		path_finder->nodes[i].closed_set = FALSE;
		path_finder->nodes[i].parents = 0;
		path_finder->nodes[i].g_score = 0;
		path_finder->nodes[i].f_score = 0;
		path_finder->nodes[i].path = FALSE;
		i = i + 1;
	}
	path_finder->has_path = FALSE;
}

void init_path_finder(struct path_finder *path_finder)
{
	INDEX_TYPE i;
	i = 0;
	while (i < PATH_FINDER_MAX_CELLS) {
		path_finder->nodes[i].open_set = FALSE;
		path_finder->nodes[i].closed_set = FALSE;
		path_finder->nodes[i].parents = 0;
		path_finder->nodes[i].g_score = 0;
		path_finder->nodes[i].f_score = 0;
		path_finder->nodes[i].path = FALSE;
		path_finder->nodes[i].passables = FALSE;
		i = i + 1;
	}
	path_finder->rows = 0;
	path_finder->cols = 0;
	path_finder->start = 0;
	path_finder->end = 0;
	path_finder->has_path = FALSE;
}
