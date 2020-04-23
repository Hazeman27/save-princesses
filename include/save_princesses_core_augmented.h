#ifndef SAVE_PRINCESSES_CORE_AUGMENTED_H
#define SAVE_PRINCESSES_CORE_AUGMENTED_H

#include <stdlib.h>
#include <ctype.h>

#define ROAD 'C'
#define BUSH 'H'
#define WALL 'N'
#define DRAK 'D'
#define PRCS 'P'

struct map {
	size_t rows;
	size_t cols;
	size_t drake_wake_time;
	char **cells;
};

#endif