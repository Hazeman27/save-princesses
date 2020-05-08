#ifndef SAVE_PRINCESSES_CORE_MAP_GENERATOR_AUGMENTED_H
#define SAVE_PRINCESSES_CORE_MAP_GENERATOR_AUGMENTED_H

#include "core_augmented.h"
#include "stack.h"

#define DIRECTIONS_COUNT 4

enum Direction { NORTH, WEST, SOUTH, EAST };

#define ERR_MSG_UNKNOWN_DIRECTION "Unknown direction"

#define PERROR_UNKNOWN_DIRECTION \
	(eprintf(ERR_MSG_UNKNOWN_DIRECTION))

_always_inline int offset_pos(int dir, int *row, int *col)
{
	switch (dir) {
		case NORTH:	return ((*row)--);
		case WEST: 	return ((*col)--);
		case SOUTH:	return ((*row)++);
		case EAST:	return ((*col)++);
		default:
			PERROR_UNKNOWN_DIRECTION;
			return -1;
	}
}

_always_inline bool out_of_bounds(struct Map *map, int dir, int row, int col)
{	
	int _row = row;
	int _col = col;
		
	if (offset_pos(dir, &_row, &_col) < 0)
		return true;
		
	if (_row < 0 || _col < 0 || _row == map->rows || _col == map->cols)
		return true;

	if (map->cells[_row][_col] == WALL)
		return true;
	
	return false;
}

_always_inline int reverse_direction(int dir)
{
	switch (dir) {
		case NORTH:	return SOUTH;
		case WEST: 	return EAST;
		case SOUTH:	return NORTH;
		case EAST:	return WEST;
		default:
			PERROR_UNKNOWN_DIRECTION;
			return -1;
	}
}

_always_inline int rotate_clockwise(int dir)
{
	switch (dir) {
		case NORTH:	return EAST;
		case WEST: 	return NORTH;
		case SOUTH:	return WEST;
		case EAST:	return SOUTH;
		default:
			PERROR_UNKNOWN_DIRECTION;
			return -1;
	}
}

_always_inline int rotate_counter_clockwise(int dir)
{
	switch (dir) {
		case NORTH:	return WEST;
		case WEST: 	return SOUTH;
		case SOUTH:	return EAST;
		case EAST:	return NORTH;
		default:
			PERROR_UNKNOWN_DIRECTION;
			return -1;
	}
}

struct Map *generate_map(int rows, int cols, int drake_wake_time);

#endif
