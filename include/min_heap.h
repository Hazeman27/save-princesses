#ifndef SAVE_PRINCESSES_MIN_HEAP_H
#define SAVE_PRINCESSES_MIN_HEAP_H

#include "rescue_mission.h"

#define ERR_MSG_HEAP_OVERFLOW "Heap overflow"

struct Heap {
	size_t size;
	size_t capacity;
	struct Node *nodes[];
};

_always_inline bool isempty(const struct Heap *heap) {
	return !heap->size;
}

struct Heap *new_heap(size_t capacity);

void min_heapify(struct Heap *heap);

bool insert(struct Heap *heap, struct Node *node);

struct Node *extract_min(struct Heap *heap);

#endif
