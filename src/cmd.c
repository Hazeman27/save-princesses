#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include "../include/core.h"

char hash(char symbol)
{
	if (!islower(symbol))
	       return -1;

	return symbol - 'a';
}

void put_command(void (*commands[])(map_t *), char symbol, const void (*command)(map_t *))
{
	char index = hash(symbol);

	if (!commands || index < 0 || commands[index])
		return;

	commands[index] = command;
}

void (*get_command(void (*commands[])(map_t *), char symbol))(map_t *)
{
	char index = hash(symbol);

	if (!commands || index < 0)
		return NULL;

	return commands[index];
}
