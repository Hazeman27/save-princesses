#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>

#include "../include/core_augmented.h"

#define SEPARATOR "----------------------\n"
#define TEST_ARGS_LABEL "> Arguments:\n"

#define print_test_label(test_name) (printf("> Running: %s\n", (test_name)))

#define _to_string(var) #var

_static_always_inline void _print_bool(bool var, const char *var_name) {
	printf("bool %s = %s", var_name, (var ? "true" : "false"));
}

_static_always_inline void _print_size_t(size_t var, const char *var_name) {
	printf("size_t %s = %ld", var_name, var);
}

_static_always_inline void _print_size_t_ptr(size_t *var, const char *var_name) {
	printf("size_t *%s = %p", var_name, var);
}

_static_always_inline void _print_char(char var, const char *var_name) {
	printf("char %s = %c", var_name, var);
}

_static_always_inline void _print_int(int var, const char *var_name) {
	printf("int %s = %d", var_name, var);
}

_static_always_inline void _print_long(long var, const char *var_name) {
	printf("long %s = %ld", var_name, var);
}

_static_always_inline void _print_float(float var, const char *var_name) {
	printf("float %s = %f", var_name, var);
}

_static_always_inline void _print_double(double var, const char *var_name) {
	printf("double %s = %lf", var_name, var);
}

_static_always_inline void _print_uchar(unsigned char var, const char *var_name) {
	printf("unsinged char %s = %u", var_name, var);
}

_static_always_inline void _print_uint(unsigned int var, const char *var_name) {
	printf("unsigned int %s = %u", var_name, var);
}

_static_always_inline void _print_ulong(unsigned long var, const char *var_name) {
	printf("unsigned long %s = %lu", var_name, var);
}

_static_always_inline void _print_char_ptr(char *var, const char *var_name) {
	printf("char *%s = %s", var_name, var);
}

_static_always_inline void _print_int_ptr(int *var, const char *var_name) {
	printf("int *%s = %p", var_name, var);
}

_static_always_inline void _print_long_ptr(long *var, const char *var_name) {
	printf("long *%s = %p", var_name, var);
}

_static_always_inline void _print_float_ptr(float *var, const char *var_name) {
	printf("float *%s = %p", var_name, var);
}

_static_always_inline void _print_double_ptr(double *var, const char *var_name) {
	printf("double *%s = %p", var_name, var);
}

_static_always_inline void _print_uchar_ptr(unsigned char *var, const char *var_name) {
	printf("unsigned char *%s = %p", var_name, var);
}

_static_always_inline void _print_uint_ptr(unsigned int *var, const char *var_name) {
	printf("unsigned int *%s = %p", var_name, var);
}

_static_always_inline void _print_ulong_ptr(unsigned long *var, const char *var_name) {
	printf("unsigned long *%s = %p", var_name, var);
}

_static_always_inline void _print_map_t_ptr(struct Map *var, const char *var_name) {
	printf("struct Map *%s = %p", var_name, var);
}

_static_always_inline void _print_map_t(struct Map var, const char *var_name) {
	printf("struct Map %s = {\n\t.rows = %ld,\n\t.cols = %ld,\n\t.drake_wake_time = %ld\n}",
					var_name, var.rows, var.cols, var.drake_wake_time);
}

_static_always_inline void _print_default(void *var, const char *var_name) {
	printf("Unknown type variable: %s = %p", var_name, var);
}

#define print_var(var, append) {				\
	_Generic(var,						\
			bool		: _print_bool,		\
			size_t		: _print_size_t, 	\
			size_t *	: _print_size_t_ptr,	\
			signed char	: _print_char,		\
			signed int	: _print_int,		\
			signed long	: _print_long,		\
			float		: _print_float,		\
			double		: _print_double,	\
			unsigned char	: _print_uchar,		\
			unsigned int	: _print_uint,		\
			signed char *	: _print_char_ptr,	\
			signed int *	: _print_int_ptr,	\
			signed long *	: _print_long_ptr,	\
			float *		: _print_float_ptr,	\
			double *	: _print_double_ptr,	\
			unsigned char *	: _print_uchar_ptr,	\
			unsigned int *	: _print_uint_ptr,	\
			struct Map *	: _print_map_t_ptr,	\
			struct Map	: _print_map_t,		\
			default		: _print_default	\
		)(var, #var);					\
	printf(append);						\
}

#define MAP_ALLOCATION_TEST_ARGC 3

static inline void check_map_against_char(const struct Map *map, char c)
{
	assert(map != NULL);

	for (size_t i = 0; i < map->rows; i++) {
		for (size_t j = 0; j < map->cols; j++)
			assert(map->cells[i][j] == c);
	}
}	

static inline
void map_allocation_test(size_t rows, size_t cols, size_t drake_wake_time, bool verbose)
{
	if (verbose) {
		printf(TEST_ARGS_LABEL);
		print_var(rows, ",\n");
		print_var(cols, ",\n");
		print_var(drake_wake_time, "\n");
		printf(SEPARATOR);
	}

	char *cells[rows];
	alloc_map_cells(cells, rows, cols);

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++)
			cells[i][j] = ROAD;
	}

	struct Map *map = new_map(rows, cols, drake_wake_time, cells);

	assert(map != NULL && map->rows == rows && map->cols == cols
			&& map->drake_wake_time == drake_wake_time);
	
	check_map_against_char(map, ROAD);

	if (verbose) print_map(map);
	free_map(map);
}

static void run_map_allocation_test(bool verbose, int runs_count, ...)
{
	assert(runs_count >= 0);
	int argc = runs_count * MAP_ALLOCATION_TEST_ARGC;

	va_list args;
	va_start(args, argc);
	
	for (int i = 0; i < argc; i += MAP_ALLOCATION_TEST_ARGC) {	
		print_test_label(_to_string(map_allocation_test));
		map_allocation_test(
				va_arg(args, size_t),
				va_arg(args, size_t),
				va_arg(args, size_t),
				verbose);
		printf(SEPARATOR "\n");
	}

	va_end(args);
}

void run_tests(bool verbose)
{
	run_map_allocation_test(verbose, 2, 5, 10, 10, 7, 100, 100);
}
