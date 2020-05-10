#include <assert.h>
#include <stdarg.h>
#include <string.h>

#include "core_augmented.h"
#include "core_utils.h"
#include "map_generator_augmented.h"

#define FLAG_VERBOSE_SHORT 	"-v"
#define FLAG_VERBOSE_FULL 	"--verbose"

#define FLAG_SHORT_L 3
#define FLAG_FULL_L 20

#define SEPARATOR "----------------------\n"
#define TEST_ARGS_LABEL "> Arguments:\n"

#define print_test_label(test_name) (printf("> Running: %s\n", (test_name)))

#define _to_string(var) #var

struct _run_3int_test_args {
	void (*test)(int, int, int, bool);
	const char *test_name;
	bool verbose;
};

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

_static_always_inline void check_map_against_char(const struct Map *map, char c)
{
	assert(map != NULL);

	for (int i = 0; i < map->rows; i++) {
		for (int j = 0; j < map->cols; j++)
			assert(map->cells[i][j] == c);
	}
}	

static void map_allocation_test(int rows, int cols, int drake_wake_time, bool verbose)
{
	struct Map *map = new_map(rows, cols, drake_wake_time);
	
	assert(map && map->rows == rows && map->cols == cols
			&& map->drake_wake_time == drake_wake_time);

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++)
			map->cells[i][j] = BUSH;
	}

	check_map_against_char(map, BUSH);

	if (verbose)
		print_map(map);
	
	free_map(map);
}

static void generate_map_test(int rows, int cols, int drake_wake_time, bool verbose)
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

_static_always_inline void copy_cells(struct Map *map, char *cells[], int rows, int cols)
{
}

static void save_princesses_test(
		char *cells[],
		int rows,
		int cols,
		int drake_t,
		int expected_path_length,
		bool verbose)
{
	if (verbose) {
		print_test_label(__func__);
		printf(TEST_ARGS_LABEL);
		print_var(cells, ",\n");
		print_var(rows, ",\n");
		print_var(cols, ",\n");
		print_var(drake_t, ",\n");
		print_var(expected_path_length, "\n");
		printf(SEPARATOR);
	}

	struct Map *map = generate_map(10, 10, 0);
	assert(map);

	int path_length;
	int *path = save_princesses(map, &path_length, false, verbose);
	
	assert(!path && !path_length);
	free_map(map);
	
	map = new_map(rows, cols, drake_t);
	
	assert(map);	
	for (int i = 0; i < rows; i++)
		for (int j = 0; j < cols; j++)
			map->cells[i][j] = cells[i][j];

	path = save_princesses(map, &path_length, false, verbose);
	assert(path && path_length == expected_path_length);

	if (verbose) {
		print_path(map, path, path_length);
		printf(SEPARATOR);
	}
	
	free_map(map);
	free(path);
}

static void run_3int_arg_test(struct _run_3int_test_args args, int argc, ...)
{
	assert(argc >= 0);

	argc *= 3;
	va_list test_args;

	va_start(test_args, argc);

	for (int i = 0; i < argc; i += 3) {	
	
		int rows = va_arg(test_args, int);
		int cols = va_arg(test_args, int);
		int drake_wake_time = va_arg(test_args, int);

		if (args.verbose) {
			print_test_label(args.test_name);
			printf(TEST_ARGS_LABEL);
			print_var(rows, ",\n");
			print_var(cols, ",\n");
			print_var(drake_wake_time, "\n");
			printf(SEPARATOR);	
		}

		args.test(rows, cols, drake_wake_time, args.verbose);
		
		if (args.verbose)
			printf(SEPARATOR "\n");
	}

	va_end(test_args);
}

_static_always_inline bool isflag(const char *str, const char *short_name, const char *full_name) {
	return (!strcmp(str, short_name) || !strcmp(str, full_name)); 
}

_static_always_inline bool accept_verbose(int argc, char **argv)
{
	if (argc < 2)
		return false;
	
	for (int i = 1; i < argc; i++) {
		if (isflag(argv[i], FLAG_VERBOSE_SHORT, FLAG_VERBOSE_FULL))
			return true;
	}
	
	return false;
}

_static_always_inline void run_scenarios(bool verbose)
{
	run_3int_arg_test((struct _run_3int_test_args) {
		map_allocation_test,
		_to_string(map_allocation_test),
		verbose
	}, 2, 10, 10, 12, 100, 100);

	run_3int_arg_test((struct _run_3int_test_args) {
		generate_map_test,
		_to_string(generate_map_test),
		verbose
	}, 2, 4, 7, 7, 12, 34, 27);


	char **cells_a = (char **) malloc(sizeof(char *) * 3);
	assert(cells_a);

	cells_a[0] = "CCCCC";
	cells_a[1] = "CCDCC";
	cells_a[2] = "CPCPC";

	save_princesses_test(cells_a, 3, 5, 4, 8, verbose);

	char **cells_b = (char **) malloc(sizeof(char *) * 5);
	assert(cells_b);

	cells_b[0] = "CNHHDP";
	cells_b[1] = "CCCCCC";
	cells_b[2] = "CCCNCC";
	cells_b[3] = "CHHNHH";
	cells_b[4] = "CHHNNC";

	save_princesses_test(cells_b, 5, 6, 7, 8, verbose);
}

int main(int argc, char **argv)
{
	srand(time(0));

	if (accept_verbose(argc, argv))
		run_scenarios(true);
	
	else run_scenarios(false);
	
	return 0;
}
