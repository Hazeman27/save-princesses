#include <string.h>

#include "../include/core_augmented.h"
#include "../include/error.h"

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

char get_cell_span(char symbol)
{
	switch (symbol) {
		case ROAD: 
		case WALL:
		case DRAK:
		case PRCS:
			return 1;
		case BUSH:
			return 2;
		default:
			return -1;
	}
}

char **new_map_cells(size_t rows, size_t cols)
{
	if (rows < 0 || cols < 0) {
		PERROR_NEG_ROWS_COLS;
		return NULL;
	}

	char **cells = (char **) malloc(sizeof(char *) * rows * cols);

	if (!cells) {
		PERROR_MALLOC;
		return NULL;
	}

	return cells;
}

void free_map_cells(char *cells[], size_t rows)
{
	if (rows < 0) {
		PERROR_NEG_ROWS_COLS;
		return;
	}

	if (!cells)
		return;

	for (size_t i = 0; i < rows; i++)
		free(cells[i]);
	free(cells); 
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
		perror("No map to scan");
		return NULL;
	}

	size_t rows;
	size_t cols;
	size_t drake_wake_time;

	char symbol;
	char span;

	if (fscanf(map_file, "%ld %ld %ld", &rows, &cols, &drake_wake_time) != 3) {
		perror(ERR_MSG_MAP_PARAMS);
		return NULL;
	}

	char **cells = new_map_cells(rows, cols);

	if (!cells)
		return NULL;

	printf("%ld %ld\n", rows, cols);

	for (size_t i = 0; i < rows; i++) {
		
		for (size_t j = 0; j < cols; j++) {

			if (fscanf(map_file, " %c", &symbol) != 1) {
				eprintf(ERR_MSG_FSCANF);
				free_map_cells(cells, rows);
				return NULL;
			}

			span = get_cell_span(symbol);

			if (span < 0) {
				eprintf(ERR_MSG_CELL_SYM);
				free_map_cells(cells, rows);
				return NULL;
			}

			cells[i][j] = symbol;
		}

		printf("%s\n", cells[i]);
		fgetc(map_file);
	}

	return new_map(rows, cols, drake_wake_time, cells);
}

struct Map *gen_map(size_t rows, size_t cols, size_t drake_wake_time)
{
	if (rows < 0 || cols < 0 || drake_wake_time < 0) {
		PERROR_NEG_ROWS_COLS;
		return NULL;
	}

	return NULL;
}

void print_map(const struct Map *map)
{
	if (!map)
		return;
	
	printf("===> Map %ld x %ld <===\n", map->rows, map->cols);

	for (size_t i = 0; i < map->rows; i++) {
		for (size_t j = 0; j < map->cols; j++)
			putchar(map->cells[i][j]);
		putchar('\n');
	}

	printf("===> Drake wake time: %ld <===\n", map->drake_wake_time);
}

void save_princesses(const struct Map *map)
{

}
