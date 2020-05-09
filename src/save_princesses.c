#include <stdlib.h>
#include <string.h>

#include "save_princesses.h"
#include "error.h"
#include "core.h"
#include "map_generator.h"
#include "cmd.h"

static void quit(map_t *map)
{
	free_map(*map);
	exit(EXIT_SUCCESS);
}

static inline void print_help(map_t *map)
{
	printf(MSG_COMMANDS);	
}

static inline void run_generate_map(map_t *map)
{
	int rows;
       	int cols;
       	int drake_wake_time;
	
	printf(MSG_ENTER_NUM_ROWS_COLS);

	if (scanf("%d %d", &rows, &cols) != 2) {
		eprintf(ERR_MSG_SCANF);
		return;		
	}
	
	if (rows < 0 || cols < 0) {
		eprintf(ERR_MSG_NEG_VAL);
		return;
	}

	printf(MSG_ENTER_DRAKE_WAKE_T);

	if (scanf("%d", &drake_wake_time) != 1) {
		eprintf(ERR_MSG_SCANF);
		return;
	}

	if (drake_wake_time < 0) {
		eprintf(ERR_MSG_NEG_VAL_DRAKE);
		return;
	}

	free_map(*map);
	*map = generate_map(rows, cols, drake_wake_time);
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

	free_map(*map);
	*map = fmake_map(map_file);
	
	if (fclose(map_file))
		eprintf(ERR_MSG_FCLOSE);
}

static void run_save_princesses(map_t *map)
{
	int path_length;
	save_princesses(*map, &path_length);
	
	eprintf(ERR_MSG_COULD_NOT_SOLVE_MAP);
	return;
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

int main(int argc, char **argv)
{
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
			run_save_princesses(&map);
			free_map(map);
		}

		return 0;
	}

interactive_mode:
	srand(time(0));	
	printf(MSG_INTERACTIVE);

	void (*commands[COMMANDS_TABLE_SIZE])(map_t *) = { NULL };
	
	put_command(commands, CMD_SYM_HELP, 		print_help);
	put_command(commands, CMD_SYM_GENERATE_MAP, 	run_generate_map);
	put_command(commands, CMD_SYM_ENTER_PATH, 	scan_map_from_file);
	put_command(commands, CMD_SYM_PRINT_MAP, 	run_print_map);
	put_command(commands, CMD_SYM_SAVE_PRINCESSES, 	run_save_princesses);
	put_command(commands, CMD_SYM_QUIT, 		quit);

	start_interactive_mode(commands);
	return 0;
}
