#ifndef SAVE_PRINCESSES_CORE_AUGMENTED_H
#define SAVE_PRINCESSES_CORE_AUGMENTED_H

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define ROAD 'C'
#define BUSH 'H'
#define WALL 'N'
#define DRAK 'D'
#define PRCS 'P'

struct Map {
	size_t rows;
	size_t cols;
	size_t drake_wake_time;
	char *cells[];
};


struct Map *new_map(size_t rows, size_t cols, size_t drake_wake_time, char *cells[]);

char get_cell_span(char symbol);

char **new_map_cells(size_t rows, size_t cols);

void free_map_cells(char *cells[], size_t rows);

void free_map(struct Map *map);

struct Map *fmake_map(FILE *map_file);

struct Map *gen_map(size_t rows, size_t cols, size_t drake_wake_time);

void print_map(const struct Map *map);

void save_princesses(const struct Map *map);

#endif
