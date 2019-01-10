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

#ifndef UASTAR_H
#define UASTAR_H

#include <stdint.h>

#ifndef PATH_FINDER_MAX_CELLS
#define PATH_FINDER_MAX_CELLS 16384
#endif


typedef int8_t    S8;
typedef int16_t   S16;
typedef int32_t   S32;
typedef int64_t   S64;
typedef uint8_t   U8;
typedef uint16_t  U16;
typedef uint32_t  U32;
typedef uint64_t  U64;
typedef float     F32;
typedef double    F64;
typedef uint8_t   BOOLEAN;
typedef uint16_t   SCORE_TYPE;/* This seems to not go above ~170 so int8 is probably enough, but for bigger than 64x64 should be 16 bit */
typedef uint16_t  INDEX_TYPE;/* This needs to hold up to PATH_FINDER_MAX_CELLS, so if bigger than 256x256 should be 32 bit */

#define SCORE_MAX 65535
#define FALSE 0U
#define TRUE (!FALSE)

/* To save RAM, the four booleans could be stored in one uint8_t as bit flags. However, this would add some time to evaluating them, so it's probably not worth it. */
typedef struct{
	BOOLEAN open_set;/* This seems to be a bool. Could be a bit flag. */
	BOOLEAN closed_set;/* This seems to be a bool. Could be a bit flag. */
	BOOLEAN path;   /* This seems to be a bool. Could be a bit flag. */
	BOOLEAN passables;/* This seems to be a bool. Could be a bit flag. */
	INDEX_TYPE parents;/* This needs to hold up to PATH_FINDER_MAX_CELLS, so if bigger than 256x256 should be 32 bit */
	SCORE_TYPE g_score;/* This seems to not go above ~140 so int8 is probably enough, but for bigger than 64x64 should be 16 bit */
	SCORE_TYPE f_score;/* This seems to not go above ~170 so int8 is probably enough, but for bigger than 64x64 should be 16 bit */
} node_type;

struct path_finder{
	INDEX_TYPE cols;
	INDEX_TYPE rows;
	INDEX_TYPE start;
	INDEX_TYPE end;
	BOOLEAN has_path;/* This seems to be a bool */
	node_type nodes[PATH_FINDER_MAX_CELLS];
	BOOLEAN (*fill_func)(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row);/* This seems to be a bool */
	SCORE_TYPE (*score_func)(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row, void *data);
	void *data;
};

SCORE_TYPE path_finder_get_heuristic_score(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row);
BOOLEAN path_finder_is_possible_target(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row);
BOOLEAN path_finder_is_passable(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row);
BOOLEAN path_finder_is_path(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row);
BOOLEAN path_finder_is_start(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row);
BOOLEAN path_finder_is_end(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row);
void path_finder_find(struct path_finder *path_finder, void *data);
void path_finder_set_path(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row, uint8_t path);
void path_finder_set_start(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row);
void path_finder_set_end(struct path_finder *path_finder, INDEX_TYPE col, INDEX_TYPE row);
void path_finder_clear_path(struct path_finder *path_finder);
void path_finder_fill(struct path_finder *path_finder);
void init_path_finder(struct path_finder *path_finder);

#endif
