#ifndef SAVE_PRINCESSES_ERROR_H
#define SAVE_PRINCESSES_ERROR_H

#include <stdio.h>
#include <errno.h>

/* Error messages */

#define ERR_MSG_FOPEN 				"Could not open map file"
#define ERR_MSG_MALLOC 				"Could not allocate memory"
#define ERR_MSG_NEG_VAL				"Rows and cols cannot have negative value"
#define ERR_MSG_NEG_VAL_DRAKE			"Drake wake time cannot be a negative value"
#define ERR_MSG_MAP_PARAMS			"Could not read map parameters"
#define ERR_MSG_READ_CELL_SYM			"Could not read cell symbol"
#define ERR_MSG_FSCANF				"Failed to read from file"
#define ERR_MSG_SCANF 				"Failed to scan input"
#define ERR_MSG_CELL_SYM 			"Incorrect symbol"
#define ERR_MSG_FCLOSE 				"Could not close file"
#define ERR_MSG_NO_MAP_FILE			"No map file to read..."
#define ERR_MSG_NO_MAP_PRINT			"No map to print..."
#define ERR_MSG_PRINCESSES_MAX_COUNT_EXCEEDED 	"Too many princesses on the map"
#define ERR_MSG_MULTI_DRAKE_DEFS 		"Multiple drakes are not allowed"
#define ERR_MSG_DRAKE_NOT_SET			"Map does not have a drake"
#define ERR_MSG_CLOCK_GETTIME			"Couldn't capture timestamp"
#define ERR_MSG_COULD_NOT_SOLVE_MAP		"Couldn't solve map and save princesses"

#define eprintf(message) (fprintf(stderr, "Error has occured in %s: %s\n", __func__, message))
#define PERROR_MALLOC (eprintf(ERR_MSG_MALLOC))

#endif
