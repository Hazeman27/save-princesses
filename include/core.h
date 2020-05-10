#ifndef SAVE_PRINCESSES_CORE_H
#define SAVE_PRINCESSES_CORE_H

#include <stdio.h>
#include <time.h>

typedef struct Map *map_t;

map_t fmake_map(FILE *map_file);

void free_map(map_t map);

int *save_princesses(map_t map, int *path_length);

void print_map(const map_t map);

void print_path(map_t map, int *path, int path_length);

#endif

