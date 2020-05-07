#include "../include/core_map_generator.h"
#include "../include/core_utils.h"
#include "../include/stack.h"

#define DIRECTIONS_COUNT 4
#define PATH_WIDTH 2

enum Direction { NORTH, WEST, SOUTH, EAST };

struct Path {
	struct Map *map;	
	struct Stack *stack;
	bool *visited[];
};

_static_always_inline void free_path(struct Path *path)
{
	for (long i = 0; i < path->map->rows; i++)
	       free(path->visited[i]);
	
	free_map(path->map);
	free(path->stack);

	free(path);
}

_static_always_inline struct Path *new_path(int rows, int cols, int drake_wake_time)
{
	struct Path *path = (struct Path *) malloc(
			sizeof(struct Path) + sizeof(bool *) * rows);

	if (!path)
		goto alloc_failure;

	path->map = new_map(rows, cols, drake_wake_time);
	path->stack = new_stack((rows * cols) << 1);

	if (!path->map || !path->stack)
		goto alloc_failure;

	for (int i = 0; i < rows; i++) {
		
		path->visited[i] = (bool *) calloc(cols, sizeof(bool));
		
		if (!path->visited[i])
			goto alloc_failure;
	}

	return path;

alloc_failure:
	free_path(path);
	PERROR_MALLOC;

	return NULL;
}

_static_always_inline bool out_of_bounds(struct Path *path, int dir, int row, int col)
{
	if ((dir == NORTH && (row - 1 < 0 || path->map->cells[row - 1][col] == WALL))
		|| (dir == WEST	 && (col - 1 < 0 		|| path->map->cells[row][col - 1] == WALL))
		|| (dir == SOUTH && (row + 1 >= path->map->rows || path->map->cells[row + 1][col] == WALL))
		|| (dir == EAST  && (col + 1 >= path->map->cols || path->map->cells[row][col + 1] == WALL))) {
			return true;
	}

	return false;
}

_static_always_inline bool is_visited(struct Path *path, int dir, int row, int col)
{	
	switch (dir) {
		case NORTH:	return path->visited[row - 1][col];
		case WEST: 	return path->visited[row][col - 1];
		case SOUTH: 	return path->visited[row + 1][col];
		case EAST: 	return path->visited[row][col + 1];
		default: 	return false;
	}
}

_static_always_inline int find_direction(struct Path *path, int row, int col)
{
	int dir, i;
	bool checked_dirs[DIRECTIONS_COUNT] = { 0 };

	do {
		dir = rand() % DIRECTIONS_COUNT;
		checked_dirs[dir] = true;
		
		for (i = 0; i < DIRECTIONS_COUNT; i++) {
			if (!checked_dirs[i])
				break;
		}

		if (i == DIRECTIONS_COUNT)
			return -1;

	} while (out_of_bounds(path, dir, row, col) || is_visited(path, dir, row, col));

	return dir;
}

_static_always_inline int push_direction(struct Path *path, int dir, int *row, int *col)
{
	switch (dir) {
		case NORTH: 	(*row)--; break;
		case WEST: 	(*col)--; break;
		case SOUTH: 	(*row)++; break;
		case EAST: 	(*col)++; break;
		default: return -1;
	}
				
	path->visited[*row][*col] = true;
	return push_pair(path->stack, *row, *col);
}

_static_always_inline void add_walls(struct Path *path, int dir, int row, int col)
{
	if (dir == NORTH) {	
		if (!out_of_bounds(path, WEST, row, col))
			path->map->cells[row][col - 1] = WALL;

		if (!out_of_bounds(path, EAST, row, col))
			path->map->cells[row][col + 1] = WALL;

		return;
	}

	if (dir == WEST) {
		if (!out_of_bounds(path, NORTH, row, col))
			path->map->cells[row - 1][col] = WALL;
		if (!out_of_bounds(path, SOUTH, row, col))
			path->map->cells[row + 1][col] = WALL;

		return;
	}

	if (dir == SOUTH) {
		if (!out_of_bounds(path, WEST, row, col))
			path->map->cells[row][col - 1] = WALL;

		if (!out_of_bounds(path, EAST, row, col))
			path->map->cells[row][col + 1] = WALL;

		return;
	}

	if (!out_of_bounds(path, NORTH, row, col))
		path->map->cells[row - 1][col] = WALL;

	if (!out_of_bounds(path, SOUTH, row, col))
		path->map->cells[row + 1][col] = WALL;
}

_static_always_inline void set_roads(struct Path *path)
{
	char road_types[] = { ROAD, ROAD, BUSH };
	int road_types_count = sizeof(road_types);

	for (int row = 0; row < path->map->rows; row++) {
		for (int col = 0; col < path->map->cols; col++) {
			
			if (path->visited[row][col]) {
				path->map->cells[row][col] = road_types[
					rand() % road_types_count
				];
			}

			else path->map->cells[row][col] = WALL;
		}
	}
}

_static_always_inline void set_princesses(struct Path *path)
{	
	int princesses_count = rand() % PRINCESSES_MAX_COUNT + 1;
	int row;
	int col;

	for (int i = 0; i < princesses_count; i++) {

		do {
			row = rand() % path->map->rows;
			col = rand() % path->map->cols;
		} while (!path->visited[row][col]);

		path->map->cells[row][col] = PRINCESS;
	}
}

_static_always_inline void set_drake(struct Path *path)
{	
	int row;
	int col;

	do {
		row = rand() % path->map->rows;
		col = rand() % path->map->cols;
	} while (!path->visited[row][col]);

	path->map->cells[row][col] = DRAKE;
}
struct Map *generate_map(int rows, int cols, int drake_wake_time)
{
	struct Path *path = new_path(rows, cols, drake_wake_time);

	if (!path)
		return NULL;
	
	int dir;
	int row = 0;
	int col = 0;

	if (push_pair(path->stack, row, col) < 0)
		goto stack_failure;

	path->visited[row][col] = true;
	
	while (path->stack->top) {
		
		dir = find_direction(path, row, col);
		
		if (dir < 0) {
			if (pop_pair(path->stack, &col, &row) < 0)
				goto stack_failure;
			continue;
		}
		
		add_walls(path, dir, row, col);
		
		if (push_direction(path, dir, &row, &col) < 0)
			goto stack_failure;
	}
	
	set_roads(path);
	set_princesses(path);
	set_drake(path);

	free(path->stack);	
	for (row = 0; row < path->map->rows; row++)
		free(path->visited[row]);

	return path->map;

stack_failure:
	free_path(path);
	return NULL;
}
