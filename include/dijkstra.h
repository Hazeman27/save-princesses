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

struct Path *find_shortest_path(struct Graph *graph, int start[2], int dest[2]);

void print_path(struct Map *map, struct Path *path);

#endif
