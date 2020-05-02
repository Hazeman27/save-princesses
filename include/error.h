#ifndef SAVE_PRINCESSES_ERROR_H
#define SAVE_PRINCESSES_ERROR_H

#include <stdio.h>
#include <errno.h>

/* Error messages */

#define ERR_MSG_FOPEN 		"Could not open map file"
#define ERR_MSG_MALLOC 		"Could not allocate memory"
#define ERR_MSG_NEG_VAL		"Rows and cols cannot have negative value"
#define ERR_MSG_NEG_VAL_DRAKE	"Drake wake time cannot be a negative value"
#define ERR_MSG_MAP_PARAMS	"Could not read map parameters"
#define ERR_MSG_READ_CELL_SYM	"Could not read cell symbol"
#define ERR_MSG_FSCANF		"Failed to read from file"
#define ERR_MSG_SCANF 		"Failed to scan input"
#define ERR_MSG_CELL_SYM 	"Incorrect symbol"
#define ERR_MSG_FCLOSE 		"Could not close file"
#define ERR_MSG_NO_MAP_FILE	"No map file to read..."
#define ERR_MSG_NO_MAP_PRINT	"No map to print..."

#define eprintf(message) (fprintf(stderr, "%s: " message "\n", __func__))

#define PERROR_MALLOC (eprintf(ERR_MSG_MALLOC))

#define PERROR_NEG_ROWS_COLS {		\
	errno = EINVAL;			\
	eprintf(ERR_MSG_NEG_VAL);	\
}

#endif
