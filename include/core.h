#ifndef SAVE_PRINCESSES_CORE_H
#define SAVE_PRINCESSES_CORE_H

#include <stdio.h>
#include <time.h>

typedef struct Map *map_t;

map_t fmake_map(FILE *map_file);

map_t generate_map(int rows, int cols, int drake_wake_time);
 
void free_map(map_t map);

int *save_princesses(map_t map, int *path_length);

void print_map(const map_t map);

#endif

