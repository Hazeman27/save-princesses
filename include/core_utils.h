#ifndef SAVE_PRINCESSES_CORE_UTILS_H
#define SAVE_PRINCESSES_CORE_UTILS_H

#include "../include/core_augmented.h"

#define CLR_RED     	"\x1b[31m"
#define CLR_GREEN  	"\x1b[32m"
#define CLR_YELLOW	"\x1b[33m"
#define CLR_BLUE    	"\x1b[34m"
#define CLR_MAGENTA 	"\x1b[35m"
#define CLR_CYAN    	"\x1b[36m"
#define CLR_RESET   	"\x1b[0m"

struct Map *new_map(int rows, int cols, int drake_wake_time);

void free_map(struct Map *map);

struct Map *fmake_map(FILE *map_file);

void print_map(const struct Map *map);

int msleep(long msec);

#endif
