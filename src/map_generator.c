#include "map_generator_augmented.h"
#include "stack.h"
#include "core_utils.h"

_static_always_inline void free_visited(bool *visited[], int rows)
{
	for (int i = 0; i < rows; i++)
	       free(visited[i]);
}

_static_always_inline bool alloc_visited(bool *visited[], int rows, int cols)
{
	for (int i = 0; i < rows; i++) {
		
		visited[i] = (bool *) calloc(cols, sizeof(bool));
		
		if (!visited[i]) {
			PERROR_MALLOC;
			free_visited(visited, rows);
			return false;
		}
	}

	return true;
}

_static_always_inline bool is_visited(bool *visited[], int dir, int row, int col)
{
	int _row = row;
	int _col = col;
	
	if (offset_pos(dir, &_row, &_col) < 0)
		return false;

	return visited[_row][_col];
}

_static_always_inline
int find_direction(struct Map *map, bool *visited[], int row, int col)
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

	} while (out_of_bounds(map, dir, row, col) || is_visited(visited, dir, row, col));

	return dir;
}

_static_always_inline
int push_direction(struct Stack *stack, bool *visited[], int dir, int *row, int *col)
{
	if (offset_pos(dir, row, col) < 0)
		return -1;

	visited[*row][*col] = true;	
	return push_pair(stack, *row, *col);
}

_static_always_inline void add_walls(struct Map *map, int dir, int row, int col)
{
	int _counter_clockwise = turn_counter_clockwise(dir);
		
	if (_counter_clockwise < 0 || out_of_bounds(map, _counter_clockwise, row, col))
		return;

	int _row = row;
	int _col = col;
			
	if (offset_pos(_counter_clockwise, &_row, &_col) < 0)
		return;

	map->cells[_row][_col] = WALL;
}

_static_always_inline void set_roads(struct Map *map, bool *visited[])
{
	char road_types[] = {
		ROAD,
		ROAD,
		BUSH
	};
	
	int road_types_count = sizeof(road_types);

	for (int row = 0; row < map->rows; row++) {
		for (int col = 0; col < map->cols; col++) {
			
			if (visited[row][col]) {
				map->cells[row][col] = road_types[
					rand() % road_types_count
				];
			}

			else map->cells[row][col] = WALL;
		}
	}
}

_static_always_inline void set_princesses(struct Map *map, bool *visited[])
{	
	int princesses_count = rand() % PRINCESSES_MAX_COUNT + 1;
	int row;
	int col;

	for (int i = 0; i < princesses_count; i++) {

		do {
			row = rand() % map->rows;
			col = rand() % map->cols;
		} while (!visited[row][col]);

		map->cells[row][col] = PRINCESS;
	}
}

_static_always_inline void set_drake(struct Map *map, bool *visited[])
{	
	int row;
	int col;

	do {
		row = rand() % map->rows;
		col = rand() % map->cols;
	} while (!visited[row][col]);

	map->cells[row][col] = DRAKE;
}

struct Map *generate_map(int rows, int cols, int drake_wake_time)
{
	struct Map *map = new_map(rows, cols, drake_wake_time);
	struct Stack *stack = new_stack((rows * cols) << 1);

	bool *visited[rows];
	
	if (!map || !stack || !alloc_visited(visited, rows, cols))
		return NULL;

	int row = 0;
	int col = 0;

	if (push_pair(stack, row, col) < 0)
		goto stack_failure;
	
	visited[row][col] = true;
	
	while (peek_top(stack)) {
		
		int dir = find_direction(map, visited, row, col);
		
		if (dir < 0) {
			if (pop_pair(stack, &col, &row) < 0)
				goto stack_failure;
			continue;
		}
		
		add_walls(map, dir, row, col);
		
		if (push_direction(stack, visited, dir, &row, &col) < 0)
			goto stack_failure;
	}
	
	set_roads(map, visited);
	set_princesses(map, visited);
	set_drake(map, visited);
	
	free_visited(visited, rows);	
	free(stack);

	return map;

stack_failure:
	free_map(map);
	free_visited(visited, rows);
	free(stack);
	return NULL;
}
