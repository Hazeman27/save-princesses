#ifndef SAVE_PRINCESSES_MIN_HEAP_H
#define SAVE_PRINCESSES_MIN_HEAP_H

#include "rescue_mission.h"

#define ERR_MSG_HEAP_OVERFLOW "Heap overflow"

struct Heap {
	size_t size;
	size_t capacity;
	struct Node *nodes[];
};

struct Heap *new_heap(size_t capacity);

int insert(struct Heap *heap, struct Node *node);

struct Node *extract_min(struct Heap *heap);

#endif
