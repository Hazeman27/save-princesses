#include "../include/save_princesses_core_augmented.h"
#include "../include/save_princesses_error.h"

static inline struct map *new_map(const struct map init)
{
	struct map *map = (struct map *) malloc(sizeof(struct map));

	if (!map) {
		PERROR_MALLOC;
		return NULL;
	}

	return (*map = init, map);
}

static inline char get_cell_span(const char symbol)
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

static inline char **new_map_cells(const size_t rows, const size_t cols)
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

static inline void free_map_cells(char **cells, const size_t rows)
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

void free_map(struct map *map)
{
	if (!map)
		return;

	free_map_cells(map->cells, map->rows);
	free(map);
}

struct map *fmake_map(FILE *map_file)
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
		perror(EMAPPRM);
		return NULL;
	}

	char **cells = new_map_cells(rows, cols);

	if (!cells)
		return NULL;

	printf("%ld %ld\n", rows, cols);

	for (size_t i = 0; i < rows; i++) {
		
		for (size_t j = 0; j < cols; j++) {

			if (fscanf(map_file, " %c", &symbol) != 1) {
				func_perror(EFSCANF);
				free_map_cells(cells, rows);
				return NULL;
			}

			span = get_cell_span(symbol);

			if (span < 0) {
				func_perror(EINSVAL);
				free_map_cells(cells, rows);
				return NULL;
			}

			cells[i][j] = symbol;
		}

		printf("%s\n", cells[i]);
		fgetc(map_file);
	}

	return new_map((struct map) { rows, cols, drake_wake_time, cells });
}

struct map *gen_map(const size_t rows, const size_t cols, const size_t drake_wake_time)
{
	if (rows < 0 || cols < 0 || drake_wake_time < 0) {
		PERROR_NEG_ROWS_COLS;
		return NULL;
	}

	return NULL;
}

void print_map(struct map *map)
{
	if (!map)
		return;

	for (size_t i = 0; i < map->rows; i++) {
		for (size_t j = 0; j < map->cols; j++)
			putchar(map->cells[i][j]);
		putchar('\n');
	}
}

void save_princesses(const struct map *map)
{

}
