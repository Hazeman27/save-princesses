#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "../include/core_augmented.h"

#define SEPARATOR "----------------------\n"
#define TEST_ARGS_LABEL "> Arguments:\n"

#define print_test_label(test_name) (printf("> Running: %s\n", (test_name)))

#define _to_string(var) #var

_static_always_inline void _print_bool(bool var, const char *var_name) {
	printf("bool %s = %s", var_name, (var ? "true" : "false"));
}

_static_always_inline void _print_char(char var, const char *var_name) {
	printf("char %s = %c", var_name, var);
}

_static_always_inline void _print_int(int var, const char *var_name) {
	printf("int %s = %d", var_name, var);
}

_static_always_inline void _print_char_ptr(char *var, const char *var_name) {
	printf("char *%s = %s", var_name, var);
}

_static_always_inline void _print_int_ptr(int *var, const char *var_name) {
	printf("int *%s = %p", var_name, var);
}

_static_always_inline void _print_map_t_ptr(struct Map *var, const char *var_name) {
	printf("struct Map *%s = %p", var_name, var);
}

_static_always_inline void _print_map_t(struct Map var, const char *var_name) {
	printf("struct Map %s = {\n\t.rows = %u,\n\t.cols = %u,\n\t.drake_wake_time = %u\n}",
			var_name, var.rows, var.cols, var.drake_wake_time);
}

_static_always_inline void _print_default(void *var, const char *var_name) {
	printf("Unknown type variable: %s = %p", var_name, var);
}

#define print_var(var, append) {				\
	_Generic(var,						\
			bool		: _print_bool,		\
			char		: _print_char,		\
			int		: _print_int,		\
			char *		: _print_char_ptr,	\
			int *		: _print_int_ptr,	\
			struct Map *	: _print_map_t_ptr,	\
			struct Map	: _print_map_t,		\
			default		: _print_default	\
		)(var, #var);					\
	printf(append);						\
}

#define MAP_ALLOCATION_TEST_ARGC 3

_static_always_inline char *iscell(char symbol) {
	return strchr(CELLS, symbol);
}

static inline void check_map_against_char(const struct Map *map, char c)
{
	assert(map != NULL);

	for (int i = 0; i < map->rows; i++) {
		for (int j = 0; j < map->cols; j++)
			assert(map->cells[i][j] == c);
	}
}	

_static_always_inline
void map_allocation_test(int rows, int cols, int drake_wake_time, bool verbose)
{
	struct Map *map = new_map(rows, cols, drake_wake_time);
	
	assert(map && map->rows == rows && map->cols == cols
			&& map->drake_wake_time == drake_wake_time);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
			map->cells[i][j] = ROAD;
	}

	check_map_against_char(map, ROAD);

	if (verbose)
		print_map(map);
	
	free_map(map);
}

_static_always_inline
void generate_map_test(int rows, int cols, int drake_wake_time, bool verbose)
{
	struct Map *map = generate_map(rows, cols, drake_wake_time);

	assert(map && map->rows == rows && map-> cols == cols
			&& map->drake_wake_time == drake_wake_time);
	
	int princesses_count = 0;
	bool drake_is_set = false;

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
			assert(validate_cell(map->cells[i][j], &princesses_count, &drake_is_set));
	}

	if (verbose)
		print_map(map);
	
	free_map(map);
}

static void run_3int_arg_test(
		void (*test)(int, int, int, bool),
		const char *test_name,
		bool verbose,
		int runs_count, ...)
{
	assert(runs_count >= 0);

	int argc = runs_count * 3;
	va_list args;

	va_start(args, argc);

	for (int i = 0; i < argc; i += 3) {	
	
		int rows = va_arg(args, int);
		int cols = va_arg(args, int);
		int drake_wake_time = va_arg(args, int);

		if (verbose) {
			print_test_label(test_name);
			printf(TEST_ARGS_LABEL);
			print_var(rows, ",\n");
			print_var(cols, ",\n");
			print_var(drake_wake_time, "\n");
			printf(SEPARATOR);	
		}

		test(rows, cols, drake_wake_time, verbose);
		
		if (verbose)
			printf(SEPARATOR "\n");
	}

	va_end(args);
}

void run_tests(bool verbose)
{
	run_3int_arg_test(
			map_allocation_test,
			_to_string(map_allocation_test),
			verbose,
			2, 5, 10, 10, 7, 100, 100);

	run_3int_arg_test(
			generate_map_test,
			_to_string(gen_map_test),
			verbose,
			2, 4, 7, 7, 12, 34, 27);
}
