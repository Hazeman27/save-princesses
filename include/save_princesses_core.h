#ifndef SAVE_PRINCESSES_CORE_H
#define SAVE_PRINCESSES_CORE_H

#include <unistd.h>

typedef struct map *map_t;


map_t fmake_map(FILE *map_file);

map_t gen_map(const size_t rows, const size_t cols, const size_t drake_wake_time);

void free_map(map_t map);

void save_princesses(map_t map);

void print_map(map_t map);

#endif