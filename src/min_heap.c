#include "min_heap.h"

_static_always_inline int parent_i(int index) {
	return (index - 1) / 2;
}

_static_always_inline int lchild_i(int index) {
	return (index << 1) + 1;
}

_static_always_inline int rchild_i(int index) {
	return (index << 1) + 2;
}

_static_always_inline int is_leaf(struct Heap *heap, int index) {
	return index >= (heap->size >> 1) && index <= heap->size;
}

_static_always_inline int _indent(int current_indent) {
	return current_indent + PRINT_INDENTATION;
}

_static_always_inline void swap(struct Node **node_a, struct Node **node_b)
{
	struct Node *tmp = *node_a;
	
	*node_a = *node_b;
	*node_b = tmp;
}

struct Heap *new_heap(size_t capacity)
{
	struct Heap *heap = (struct Heap *) malloc(
			sizeof(struct Heap) + sizeof(struct Node *) * capacity);

	if (!heap) {
		PERROR_MALLOC;
		return NULL;
	}

	*heap = (struct Heap) { 0, capacity };
	return heap;
}

static void heapify(struct Heap *heap, int index)
{
	if (is_leaf(heap, index))
		return;
	
	int left = lchild_i(index);
	int right = rchild_i(index);

	if (heap->nodes[index]->priority <= heap->nodes[left]->priority && 
			heap->nodes[index]->priority <= heap->nodes[right]->priority)
		return;

	if (heap->nodes[left]->priority < heap->nodes[right]->priority) {
		
		swap(&heap->nodes[index], &heap->nodes[left]);
		heapify(heap, left);
		
		return;
	}

	swap(&heap->nodes[index], &heap->nodes[right]);
	heapify(heap, right);
}

void min_heapify(struct Heap *heap)
{
	if (!heap)
		return;

	for (int i = heap->size / 2; i >= 0; i--)
		heapify(heap, i);
}

bool insert(struct Heap *heap, struct Node *node)
{
	if (!heap || !node)
		return false;

	if (heap->size == heap->capacity) {
		eprintf(ERR_MSG_HEAP_OVERFLOW);
		return false;
	}

	heap->nodes[heap->size] = node; 
	heap->size++;

	int current = heap->size - 1;
	
	while (heap->nodes[current]->priority < heap->nodes[parent_i(current)]->priority) {
		swap(&heap->nodes[current], &heap->nodes[parent_i(current)]);
		current = parent_i(current);
	}

	return true;
}

struct Node *extract_min(struct Heap *heap)
{
	if (!heap)
		return NULL;

	struct Node *node = heap->nodes[0];
	
	heap->size--;
	heap->nodes[0] = heap->nodes[heap->size];

	heapify(heap, 0);
	return node;
}

void print_heap(const struct Heap *heap, int index, int indent)
{
	if (!heap || index >= heap->size)
		return;

	for (int i = 0; i < indent; i++) {
		if ((i + 1) % PRINT_INDENTATION)
			putchar(' ');
		else putchar('|');
	}
	
	putchar('-');
	printf("[%d]\n", heap->nodes[index]->priority);

	print_heap(heap, lchild_i(index), _indent(indent));
	print_heap(heap, rchild_i(index), _indent(indent));
}
