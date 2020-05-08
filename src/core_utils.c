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

void free_map(struct Map *map)
{
	if (!map)
		return;

	for (int i = 0; i < map->rows; i++)
		free(map->cells[i]);
	
	free(map);
}

struct Map *new_map(int rows, int cols, int drake_wake_time)
{
	struct Map *map = (struct Map *) malloc(
			sizeof(struct Map) + sizeof(char *) * rows);

	if (!map)
		goto alloc_failure;

	*map = (struct Map) { rows, cols, drake_wake_time };
	
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

	printf("===> Map %u x %u <===\n", map->rows, map->cols);

	for (int i = 0; i < map->rows; i++) {
		for (int j = 0; j < map->cols; j++) {	
			
			switch (map->cells[i][j]) {
				case PRINCESS: printf(CLR_GREEN); break;
				case DRAKE: printf(CLR_RED); break;
				default: break;
			}
			
			putchar(map->cells[i][j]);
			printf(CLR_RESET);
		}
		putchar('\n');
	}

	printf("===> Drake wake time: %u <===\n", map->drake_wake_time);
}


/**
 * Thread sleep in miliseconds function. Implementation by 'caf'
 * https://stackoverflow.com/a/1157217/13391945
 */

int msleep(long msec)
{
	struct timespec ts;
	int res;

	if (msec < 0) {
		errno = EINVAL;
		return -1;
	}

	ts.tv_sec = msec / 1000;
	ts.tv_nsec = (msec % 1000) * 1000000;

	do {
		res = nanosleep(&ts, &ts);
	} while (res && errno == EINTR);

	return res;
}
