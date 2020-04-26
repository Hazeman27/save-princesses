#include <stdlib.h>

#include "../include/save_princesses.h"
#include "../include/save_princesses_error.h"
#include "../include/save_princesses_core.h"
#include "../include/save_princesses_cmd.h"

static void quit(map_t map)
{
	free_map(map);
	exit(EXIT_SUCCESS);
}

static void print_help(map_t map)
{
	printf(MSG_COMMANDS);	
}

static inline void reset_map(map_t map)
{
	if (map != NULL)
		free_map(map);
}

static void generate_map(map_t map)
{
	size_t rows, cols, drake_wake_time;

	if (scanf("%ld %ld %ld", &rows, &cols, &drake_wake_time) != 3) {
		func_perror(ERR_MSG_SCANFF);
		return;		
	}	

	reset_map(map);
	map = gen_map(rows, cols, drake_wake_time);
}

static inline FILE *scan_map_file_path()
{
	char path[MAP_PATH_MAX_L];
	
	if (scanf("%s", path) != 1) {
		func_perror(ERR_MSG_SCANFF);
		return NULL;
	}

	return fopen(path, "r");
}

static void scan_map_from_file(map_t map)
{
	FILE *map_file = scan_map_file_path();

	if (!map_file) {
		func_perror(ERR_MSG_FOPEN);
		return;	
	}

	reset_map(map);
	map = fmake_map(map_file);

	if (fclose(map_file) == EOF)
		func_perror(ERR_MSG_FCLOSE);
}

static inline void start_iteraction_mode(void (*commands[])(map_t))
{
	if (!commands)
		return;
	
	map_t map = NULL;
	char input;

	printf(MSG_ENTER_CMD);

	while (scanf("%c", &input) == 1) {
		
		if (input == '\n')
			continue;

		void (*command)(map_t) = get_command(commands, input);
		
		if (command)
			command(map);

		printf(MSG_ENTER_CMD);	
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
			func_perror(ERR_MSG_FCLOSE);
			return -1;
		}

		if (map)
			save_princesses(map);
		goto quit;
	}

interactive_mode:
	printf(MSG_INTERACTIVE);

	void (*commands[COMMANDS_TABLE_SIZE])(map_t) = { NULL };
	
	put_command(commands, CMD_SYM_HELP, 		print_help);
	put_command(commands, CMD_SYM_GENERATE_MAP, 	generate_map);
	put_command(commands, CMD_SYM_ENTER_PATH, 	scan_map_from_file);
	put_command(commands, CMD_SYM_PRINT_MAP, 	print_map);
	put_command(commands, CMD_SYM_SAVE_PRINCESSES, 	save_princesses);
	put_command(commands, CMD_SYM_QUIT, 		quit);

	start_iteraction_mode(commands);
quit:
	return 0;
}
