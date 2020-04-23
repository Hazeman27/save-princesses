#ifndef SAVE_PRINCESSES_ERROR_H
#define SAVE_PRINCESSES_ERROR_H

#include <stdio.h>
#include <errno.h>

/* Error messages */

#define EMAPNFD "Map file not found\n"
#define EMALLOC "Could not allocate memory\n"
#define ENEGRCV	"Rows and cols cannot have negative value\n"
#define EMAPPRM	"Could not read map parameters\n"
#define EMAPCLL	"Could not read cell symbol\n"
#define EFSCANF	"Failed to read from file\n"
#define ESCANFF "Failed to scan input\n"
#define EINSVAL "Incorrect symbol\n"
#define EFILECL "Could not close file\n"

#define func_perror(messages...) (fprintf(stderr, "%s: " messages, __func__))

#define PERROR_MALLOC (func_perror(EMALLOC))

#define PERROR_NEG_ROWS_COLS {	\
	errno = EINVAL;				\
	func_perror(ENEGRCV);		\
}

#endif