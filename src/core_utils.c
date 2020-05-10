#include "core_utils.h"

_static_always_inline void skip_whitespace(FILE *file, char *c) {
	do { *c = fgetc(file); } while (isspace(*c));
}

_static_always_inline bool fscan_map(FILE *map_file, struct Map *map)
{
	char cell;
	bool drake_is_set = false;
	int princesses_count = 0;

	for (int i = 0; i < map->rows; i++) {
		for (int j = 0; j < map->cols; j++) {
			
			skip_whitespace(map_file, &cell);
			
			if (validate_cell(cell, &princesses_count, &drake_is_set) < 0)
				return false;

			map->cells[i][j] = cell;
		}
	}
	
	if (!drake_is_set) {
		eprintf(ERR_MSG_DRAKE_NOT_SET);
		return false;
	}

	return true;
}

void free_cells(char *cells[], int rows)
{
	for (int i = 0; i < rows; i++)
		free(cells[i]);
}

void free_map(struct Map *map)
{
	if (!map)
		return;

	free_cells(map->cells, map->rows);
	free(map);
}

struct Map *new_map(int rows, int cols, int drake_wake_time)
{
	struct Map *map = (struct Map *) malloc(
			sizeof(struct Map) + sizeof(char *) * rows);

	if (!map)
		goto alloc_failure;

	*map = (struct Map) {
		rows,
		cols,
		drake_wake_time
	};
	
	for (int i = 0; i < rows; i++) {
		map->cells[i] = (char *) calloc(cols, sizeof(char));

		if (!map->cells[i])
			goto alloc_failure;
	}

	return map;

alloc_failure:	
	free_map(map);
	PERROR_MALLOC;

	return NULL;
}

struct Map *copy_map(const struct Map *map)
{
	struct Map *copy = new_map(map->rows, map->cols, map->drake_wake_time);

	if (!copy)
		return NULL;

	for (int i = 0; i < map->rows; i++)
		memcpy(copy->cells[i], map->cells[i], sizeof(char) * map->cols);

	return copy;
}

struct Map *fmake_map(FILE *map_file)
{
	if (!map_file) {
		eprintf(ERR_MSG_NO_MAP_FILE);
		return NULL;
	}

	int rows;
	int cols;
	int drake_wake_time;
	
	if (fscanf(map_file, "%d %d %d", &rows, &cols, &drake_wake_time) != 3) {
		eprintf(ERR_MSG_MAP_PARAMS);
		return NULL;
	}
	
	struct Map *map = new_map(rows, cols, drake_wake_time);

	if (!map || !fscan_map(map_file, map)) {
		free_map(map);
		return NULL;
	}

	return map;
}

void print_map(const struct Map *map)
{
	if (!map) {
		eprintf(ERR_MSG_NO_MAP_PRINT);
		return;
	}
	
	bool pretty_print = true;

	if (map->cols > 25)
		pretty_print = false;

	printf(">> Map %u x %u\n", map->rows, map->cols);
	printf(">> Drake's wake time: %u\n", map->drake_wake_time);
	
	if (pretty_print) {
		printf("   ");

		for (int i = 0; i < map->cols; i++)
			printf(CLR_YELLOW "%02d " RESET, i);
		printf("\n");
	}
	
	for (int i = 0; i < map->rows; i++) {
		
		if (pretty_print)
			printf(CLR_YELLOW "%02d " RESET, i);

		for (int j = 0; j < map->cols; j++) {	
			
			switch (map->cells[i][j]) {
				case WALL: printf(CLR_BLUE); break;
				case PATH: printf(CLR_CYAN); break;
				case PATH_KEY: printf(CLR_MAGENTA); break;
				case PRINCESS: printf(TXT_UNDERLINE CLR_GREEN); break;
				case DRAKE: printf(TXT_UNDERLINE CLR_RED); break;
				default: break;
			}
			
			if (pretty_print)
				printf(" ");

			printf("%c" RESET, map->cells[i][j]);
			
			if (pretty_print)
				printf(" ");
		}

		putchar('\n');
	}
}

void print_path(struct Map *map, int *path, int path_length)
{
	if (!map || !path)
		return;
	
	struct Map *copy = copy_map(map);

	if (!copy)
		return;
	
	printf(">> Path:\n\tLength: %d\n", path_length);

	for (int i = 0; i < path_length; i++) {

		int row = path[(i << 1)];
		int col = path[(i << 1) + 1];
		
		if (copy->cells[row][col] == DRAKE || copy->cells[row][col] == PRINCESS)
			copy->cells[row][col] = PATH_KEY;
		else copy->cells[row][col] = PATH;
	}

	print_map(copy);
	free_map(copy);
}
