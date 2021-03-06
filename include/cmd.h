#ifndef SAVE_PRINCESSES_CMD_H
#define SAVE_PRINCESSES_CMD_H

#define COMMANDS_TABLE_SIZE 25

#include "core.h"

extern void put_command(void (*commands[])(map_t *), char symbol, void (*command)(map_t *));

extern void (*get_command(void (*commands[])(map_t *), char symbol))(map_t *);

#endif
