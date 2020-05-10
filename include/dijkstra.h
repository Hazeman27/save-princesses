#ifndef SAVE_PRINCESSES_DIJKSTRA_H
#define SAVE_PRINCESSES_DIJKSTRA_H

#define PATH_INITIAL_SIZE 32

#include "rescue_mission.h"

struct Path {
	int size;
	int length;
	int time_complexity;
	int *trace;
};

_always_inline void free_path(struct Path *path)
{
	free(path->trace);
	free(path);
}

_always_inline int *reverse_trace(int *trace, int length, bool reverse_axis)
{
	int *reversed = (int *) malloc(sizeof(int) * (length << 1));

	if (!reversed) {
		PERROR_MALLOC;
		return NULL;
	}
	
	int index = length << 1;

	for (int i = 0; i < length; i++) {
		reversed[index - 2 - reverse_axis - (i << 1)] = trace[i << 1];
		reversed[index - 1 + reverse_axis - (i << 1)] = trace[(i << 1) + 1];
	}

	return reversed;
}

struct Path *concat_paths(const struct Path *path_a, const struct Path *path_b);

struct Path *find_shortest_path(struct Graph *graph, int start[2], int dest[2]);

#endif
