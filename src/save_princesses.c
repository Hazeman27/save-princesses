#include <stdlib.h>
#include <string.h>

#include "../include/save_princesses.h"
#include "../include/error.h"
#include "../include/core.h"
#include "../include/cmd.h"
#include "../include/core_assert.h"

static void quit(map_t *map)
{
	free_map(*map);
	exit(EXIT_SUCCESS);
}

static inline void print_help(map_t *map)
{
	printf(MSG_COMMANDS);	
}

static inline void reset_map(map_t map)
{
	if (map != NULL)
		free_map(map);
}

static inline void generate_map(map_t *map)
{
	size_t rows, cols, drake_wake_time;
	
	printf(MSG_ENTER_NUM_ROWS_COLS);

	if (scanf("%ld %ld", &rows, &cols) != 2) {
		eprintf(ERR_MSG_SCANF);
		return;		
	}
	
	if (rows < 0 || cols < 0) {
		PERROR_NEG_ROWS_COLS;
		return;
	}

	printf(MSG_ENTER_DRAKE_WAKE_T);

	if (scanf("%ld", &drake_wake_time) != 1) {
		eprintf(ERR_MSG_SCANF);
		return;
	}

	if (drake_wake_time < 0) {
		eprintf(ERR_MSG_NEG_VAL_DRAKE);
		return;
	}

	reset_map(*map);
	*map = gen_map(rows, cols, drake_wake_time);
}


static inline void scan_map_from_file(map_t *map)
{
	char path[MAP_PATH_MAX_L];
	
	if (scanf("%s", path) != 1) {
		eprintf(ERR_MSG_SCANF);
		return;
	}

	FILE *map_file = fopen(path, "r");

	if (!map_file) {
		eprintf(ERR_MSG_FOPEN);
		return;	
	}	

	reset_map(*map);
	*map = fmake_map(map_file);
	
	if (fclose(map_file))
		eprintf(ERR_MSG_FCLOSE);
}

static void run_save_princesses(map_t *map)
{
	save_princesses(*map);
}

static void run_print_map(map_t *map)
{
	print_map(*map);
}

static void start_interactive_mode(void (*commands[])(map_t *))
{
	if (!commands)
		return;
	
	map_t map = NULL;
	char input;

	printf(MSG_COMMANDS MSG_ENTER_CMD);

	while (scanf("%c", &input) == 1) {
		
		if (input == '\n')
			continue;

		void (*command)(map_t *) = get_command(commands, input);
		
		if (command)
			command(&map);

		printf("\nsave_princesses>");	
	}
}

static inline
bool isflag(const char *str, const char *flag_short_name, const char *flag_full_name)
{
	return (!strcmp(str, flag_short_name) || !strcmp(str, flag_full_name)); 
}

static void accept_program_flags(int argc, char **argv)
{
	if (argc < 2)
		return;
	
	if (argc == 2 && isflag(argv[1], FLAG_ASSERT_SHORT, FLAG_ASSERT_FULL)) {
		run_tests(false);
		return;	
	}
	
	bool verbose = false;
	bool run = false;

	for (int i = 1; i < argc; i++) {
		
		if (isflag(argv[i], FLAG_VERBOSE_SHORT, FLAG_VERBOSE_FULL))
			verbose = true;
		else if (isflag(argv[i], FLAG_ASSERT_SHORT, FLAG_ASSERT_FULL))
			run = true;

		if (run && verbose) {
			run_tests(true);
			break;
		}
	}

	if (run)
		run_tests(false);
}

int main(int argc, char **argv)
{
	accept_program_flags(argc, argv);

	if (argc > 1) {
		
		FILE *map_file = fopen(argv[1], "r");

		if (!map_file) {
			perror(ERR_MSG_FOPEN);
			goto interactive_mode;
		}
		
		map_t map = fmake_map(map_file);
				
		if (fclose(map_file) == EOF) {
			eprintf(ERR_MSG_FCLOSE);
			return -1;
		}

		if (map) {
			save_princesses(map);
			free_map(map);
		}

		return 0;
	}

interactive_mode:
	srand(time(0));	
	printf(MSG_INTERACTIVE);

	void (*commands[COMMANDS_TABLE_SIZE])(map_t *) = { NULL };
	
	put_command(commands, CMD_SYM_HELP, 		print_help);
	put_command(commands, CMD_SYM_GENERATE_MAP, 	generate_map);
	put_command(commands, CMD_SYM_ENTER_PATH, 	scan_map_from_file);
	put_command(commands, CMD_SYM_PRINT_MAP, 	run_print_map);
	put_command(commands, CMD_SYM_SAVE_PRINCESSES, 	run_save_princesses);
	put_command(commands, CMD_SYM_QUIT, 		quit);

	start_interactive_mode(commands);
	return 0;
}
