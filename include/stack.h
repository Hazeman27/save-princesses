#ifndef SAVE_PRINCESSES_STACK_H
#define SAVE_PRINCESSES_STACK_H

#include <unistd.h>

typedef struct Stack *stack_t;

stack_t new_stack(size_t size);

int push(stack_t stack, int item);

int push_pair(stack_t stack, int item_a, int item_b);

int pop(stack_t stack);

int pop_pair(stack_t stack, int *pop_a, int *pop_b);

int peek_top(stack_t stack);

#endif
