#ifndef SAVE_PRINCESSES_H
#define SAVE_PRINCESSES_H

#define MAP_PATH_MAX_L sizeof(char)

/* Regular messages */

#define MSG_NARG "Map file not specified.\n"
#define MSG_IOMD "Switching to interactive mode...\n"
#define MSG_CMDS "Commands:\n\t0. Print commands [h].\n\t1. Generate map [g].\n\t2. Enter map file path [f].\n\t3. Print current map [p].\n\t4. Save princesses [s].\n\t5. Quit [q].\n\n"
#define MSG_ECHO "Enter your choice: "
#define MSG_EPTH "Enter path to map file: "
#define MSG_ERCN "Enter number of rows and cols [r c]: "
#define MSG_EDTN "Enter Drake's wake time parameter: "
#define MSG_GENS "Map generated successfully...\n"

/* Commands */

#define CMD_HELP 'h'
#define CMD_GENM 'g'
#define CMD_PATH 'f'
#define CMD_PRNT 'p'
#define CMD_SAVE 's'
#define CMD_QUIT 'q'

#endif