#ifndef SAVE_PRINCESSES_H
#define SAVE_PRINCESSES_H

#include <unistd.h>

#define MAP_PATH_MAX_L 256

/* Regular messages */

#define MSG_MAP_NSPEC 		"Map file not specified.\n"
#define MSG_INTERACTIVE 	"Switching to interactive mode...\n"

/* Commands */

#define CMD_SYM_HELP 		'h'
#define CMD_SYM_GENERATE_MAP 	'g'
#define CMD_SYM_ENTER_PATH 	'f'
#define CMD_SYM_PRINT_MAP 	'p'
#define CMD_SYM_SAVE_PRINCESSES 's'
#define CMD_SYM_QUIT 		'q'

#define CMD_1 "1. Print commands      [h]."
#define CMD_2 "2. Generate map        [g]."
#define CMD_3 "3. Enter map file path [f [filepath]]."
#define CMD_4 "4. Print current map   [p]."
#define CMD_5 "5. Save princesses     [s]."
#define CMD_6 "6. Quit                [q]."

#define CMDS CMD_1 LT CMD_2 LT CMD_3 LT CMD_4 LT CMD_5 LT CMD_6 
#define LT "\n\t"

#define MSG_COMMANDS 		"Commands:" LT CMDS "\n"
#define MSG_ENTER_CMD 		"Enter your choice: "
#define MSG_ENTER_PATH 		"Enter path to map file: "
#define MSG_ENTER_NUM_ROWS_COLS "Enter number of rows and cols [r c]: "
#define MSG_ENTER_DRAKE_WAKE_T 	"Enter Drake's wake time parameter: "
#define MSG_MAP_GENERATED 	"Map generated successfully...\n"

#define FLAG_ASSERT_SHORT 	"-a"
#define FLAG_VERBOSE_SHORT 	"-v"
#define FLAG_ASSERT_FULL 	"--assert"
#define FLAG_VERBOSE_FULL 	"--verbose"

#define FLAG_SHORT_L 3
#define FLAG_FULL_L 20

#endif
