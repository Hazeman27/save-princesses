#ifndef SAVE_PRINCESSES_CORE_H
#define SAVE_PRINCESSES_CORE_H

#include <stdio.h>
#include <stdbool.h>
#include <time.h>

typedef struct Map *map_t;

map_t fmake_map(FILE *map_file);

void free_map(map_t map);

int *save_princesses(map_t map, int *path_length, bool reverse_axis, bool verbose);

void print_map(const map_t map);

void print_path(map_t map, int *path, int path_length);

/**
 * This function name is mandatory due to this being a university project. Despite that,
 * it pains me that I have to incorporate Slovak language into my code...
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * This function name, it hurts me deeply,
 * Despair and sorrow fills my soul,
 * At least, let my ashes speak freely,
 * Why can't I rename this evil spawn?
 *
 * - A Lone Coder, 2020
 */

int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty);

#endif

