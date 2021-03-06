#ifndef SAVE_PRINCESSES_CORE_UTILS_H
#define SAVE_PRINCESSES_CORE_UTILS_H

#include "core_augmented.h"

#define CLR_RED     	"\x1b[31m"
#define CLR_GREEN  	"\x1b[32m"
#define CLR_YELLOW	"\x1b[33m"
#define CLR_BLUE    	"\x1b[34m"
#define CLR_MAGENTA 	"\x1b[35m"
#define CLR_CYAN    	"\x1b[36m"

#define BG_RED		"\x1b[41m"
#define BG_GREEN	"\x1b[42m"

#define TXT_UNDERLINE	"\x1b[4m"
#define RESET   	"\x1b[0m"

void free_cells(char *cells[], int rows);

void free_map(struct Map *map);

struct Map *new_map(int rows, int cols, int drake_wake_time);

struct Map *copy_map(const struct Map *map);

struct Map *fmake_map(FILE *map_file);

void print_map(const struct Map *map);

void print_path(struct Map *map, int *path, int path_length);

int msleep(long msec);

#endif
