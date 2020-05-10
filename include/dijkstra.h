#ifndef SAVE_PRINCESSES_DIJKSTRA_H
#define SAVE_PRINCESSES_DIJKSTRA_H

#define PATH_INITIAL_SIZE 16

#include "rescue_mission.h"

struct Cell {
	char span;
	int row;
	int col;
};

struct Path {
	int size;
	int length;
	int time_complexity;
	struct Cell *cells;
};

_always_inline void free_path(struct Path *path)
{
	free(path->cells);
	free(path);
}

_always_inline int *extract_trace(struct Path *path, bool reverse_axis)
{
	int *reversed = (int *) malloc(sizeof(int) * (path->length << 1));

	if (!reversed) {
		PERROR_MALLOC;
		return NULL;
	}
	
	int index = path->length << 1;

	for (int i = 0; i < path->length; i++) {

		struct Cell cell = path->cells[i];

		reversed[index - 2 - reverse_axis - (i << 1)] = cell.row;
		reversed[index - 1 + reverse_axis - (i << 1)] = cell.col;
	}

	return reversed;
}

struct Path *find_shortest_path(struct Graph *graph, int start[2], int dest[2]);

struct Path *concat_paths(const struct Path *path_a, const struct Path *path_b);

#endif
