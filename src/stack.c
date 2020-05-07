#include <stdlib.h>

#include "../include/stack.h"
#include "../include/error.h"

#define ERR_MSG_STACK_OVERFLOW "Stack overflow"
#define ERR_MSG_STACK_UNDERFLOW "Stack underflow"

struct Stack *new_stack(size_t size)
{
	struct Stack *stack = (struct Stack *) malloc(
			sizeof(struct Stack) + sizeof(int) * size);

	if (!stack) {
		PERROR_MALLOC;
		return NULL;
	}

	*stack = (struct Stack) { size, 0 };
	return stack;
}

int push(struct Stack *stack, int item)
{
	if (stack->top == stack->size) {
		eprintf(ERR_MSG_STACK_OVERFLOW);
		return -1;
	}

	stack->items[stack->top] = item;
	stack->top++;

	return 0;
}

int push_pair(struct Stack *stack, int item_a, int item_b)
{
	if (push(stack, item_a) < 0)
		return -1;

	return push(stack, item_b);
}

int pop(struct Stack *stack)
{
	if (!stack->top) {
		eprintf(ERR_MSG_STACK_UNDERFLOW);
		return -1;
	}

	stack->top--;
	return stack->items[stack->top];
}

int pop_pair(struct Stack *stack, int *pop_a, int *pop_b)
{
	if ((*pop_a = pop(stack)) < 0)
		return -1;

	return *pop_b = pop(stack);
}
