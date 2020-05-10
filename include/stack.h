#ifndef SAVE_PRINCESSES_STACK_H
#define SAVE_PRINCESSES_STACK_H

#include <stdlib.h>
#include "error.h"

#define ERR_MSG_STACK_OVERFLOW "Stack overflow"
#define ERR_MSG_STACK_UNDERFLOW "Stack underflow"

struct Stack {
	size_t size;
	size_t top;	
	int items[];
};

struct Stack *new_stack(size_t size);

int push(struct Stack *stack, int item);

int push_pair(struct Stack *stack, int item_a, int item_b);

int pop(struct Stack *stack);

int pop_pair(struct Stack *stack, int *pop_a, int *pop_b);

int peek_top(struct Stack *stack);

#endif
