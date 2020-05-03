#include <string.h>
#include <time.h>

#include "../include/core_augmented.h"

_static_always_inline void skip_whitespace(FILE *file, char *c)
{
	do { *c = fgetc(file); } while (isspace(*c));
}

_static_always_inline bool fscan_map_cells(FILE *map_file, char *cells[], size_t rows, size_t cols)
{
	char cell;
	int princesses_count = 0;
	bool drake_is_set = false;

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			
			skip_whitespace(map_file, &cell);
			
			if (validate_cell(cell, &princesses_count, &drake_is_set, true) < 0)
				return false;

			cells[i][j] = cell;
		}
	}

	return true;
}

struct Map *new_map(size_t rows, size_t cols, size_t drake_wake_time, char *cells[])
{
	struct Map *map = (struct Map *) malloc(sizeof(struct Map) + sizeof(char *) * rows);

	if (!map) {
		PERROR_MALLOC;
		return NULL;
	}

	*map = (struct Map) { rows, cols, drake_wake_time };
	memmove(map->cells, cells, sizeof(char *) * rows);

	return map;
}

void alloc_map_cells(char *cells[], size_t rows, size_t cols)
{
	for (size_t i = 0; i < rows; i++) {

		cells[i] = (char *) calloc(cols, sizeof(char));

		if (!cells[i]) {
			PERROR_MALLOC;
			cells[0] = NULL;
			return;
		}
	}
}

void free_map_cells(char *cells[], size_t rows)
{
	if (!cells)
		return;

	for (size_t i = 0; i < rows; i++)
		free(cells[i]);
}

void free_map(struct Map *map)
{
	if (!map)
		return;

	free_map_cells(map->cells, map->rows);
	free(map);
}

struct Map *fmake_map(FILE *map_file)
{
	if (!map_file) {
		eprintf(ERR_MSG_NO_MAP_FILE);
		return NULL;
	}

	size_t rows, cols, drake_wake_time;
	
	if (fscanf(map_file, "%ld %ld %ld", &rows, &cols, &drake_wake_time) != 3) {
		eprintf(ERR_MSG_MAP_PARAMS);
		return NULL;
	}

	char *cells[rows];
	alloc_map_cells(cells, rows, cols + 1);

	if (!cells[0])
		return NULL;
	
	if (!fscan_map_cells(map_file, cells, rows, cols)) {
		free_map_cells(cells, rows);
		return NULL;
	}

	struct Map *map = new_map(rows, cols, drake_wake_time, cells);
	return map;
}

struct Map *gen_map(size_t rows, size_t cols, size_t drake_wake_time)
{
	srand(time(0));
	
	/**
	 * Duplicate elements to increase probability of
	 * cell being that type
	 */

	char cell_types[] = { ROAD, ROAD, ROAD, BUSH, WALL, DRAKE, PRINCESS };
	int cell_types_amount = strlen(cell_types);

	char *cells[rows];
	alloc_map_cells(cells, rows, cols + 1);

	if (!cells[0])
		return NULL;
	
	/**
	 * princesses_count - Count amount of princesses,
	 * max allowed amount is specified by
	 * PRINCESSES_MAX_COUNT in the header file
	 * core_augmented.h
	 *
	 * drake_is_set - check if DRAKE was already set,
	 * max allowed amount of drakes is 1
	 */

	int princesses_count = 0;
	bool drake_is_set = false;
	
	char cell;

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			
			/**
			 * If cell is PRINCESS and princesses max amount is
			 * already reached or cell is DRAKE and drake is
			 * already set, then keep selecting random cell types
			 */

			do {
				cell = cell_types[rand() % cell_types_amount];
			} while (validate_cell(cell, &princesses_count, &drake_is_set, false) < 0);
			
			cells[i][j] = cell;
		}
	}

	struct Map *map = new_map(rows, cols, drake_wake_time, cells);
	return map;
}

void print_map(const struct Map *map)
{
	if (!map) {
		eprintf(ERR_MSG_NO_MAP_PRINT);
		return;
	}

	char delimeter = ' ';

	if (map->cols > 30)
		delimeter = 0;

	printf("===> Map %ld x %ld <===\n", map->rows, map->cols);

	for (size_t i = 0; i < map->rows; i++) {
		for (size_t j = 0; j < map->cols; j++)
			printf("%c%c", map->cells[i][j], delimeter);
		putchar('\n');
	}

	printf("===> Drake wake time: %ld <===\n", map->drake_wake_time);
}

_static_always_inline int count_princesses(char *cells[], size_t rows, size_t cols)
{
	int count = 0;

	struct timespec start, end;
	clock_gettime(CLOCK_REALTIME, &start);

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			if (cells[i][j] == PRINCESS)
				count++;
		}
	}

	clock_gettime(CLOCK_REALTIME, &end);
	print_delta_time(stdin, calc_delta_time(start, end));

	return count;
}

void save_princesses(struct Map *map)
{
	if (!map)
		return;

	int princesses_count = count_princesses(map->cells, map->rows, map->cols);
	printf("Princesses count: %d\n", princesses_count);	
}
