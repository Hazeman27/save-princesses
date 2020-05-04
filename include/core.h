#ifndef SAVE_PRINCESSES_CORE_H
#define SAVE_PRINCESSES_CORE_H

#include <stdio.h>
#include <unistd.h>
#include <time.h>

typedef struct Map *map_t;

map_t fmake_map(FILE *map_file);

map_t gen_map(size_t rows, size_t cols, size_t drake_wake_time);
 
void free_map(map_t map);

void save_princesses(map_t map);

void print_map(const map_t map);

#endif

