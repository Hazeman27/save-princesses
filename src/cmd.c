#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include "core.h"

#define NIL (char) -1
#define isnil(value) ((char) (value) == NIL)

unsigned char hash(char symbol)
{
	if (!islower(symbol))
	       return NIL;
	
	return symbol - 'a';
}

void put_command(void (*commands[])(map_t *), char symbol, void (*command)(map_t *))
{
	unsigned char index = hash(symbol);

	if (!commands || isnil(index) || commands[index])
		return;

	commands[index] = command;
}

void (*get_command(void (*commands[])(map_t *), char symbol))(map_t *)
{
	unsigned char index = hash(symbol);

	if (!commands || isnil(index))
		return NULL;

	return commands[index];
}
