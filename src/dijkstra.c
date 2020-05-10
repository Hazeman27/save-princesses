#include "min_heap.h"
#include "dijkstra.h"

_static_always_inline void resize_path(struct Path *path)
{
	if (!path || (path->length << 1) < path->size)
		return;

	path->size <<= 1;
	path->trace = (int *) realloc(path->trace, sizeof(int) * (path->size));
}

_static_always_inline struct Path *new_path(int initial_size)
{
	struct Path *path = (struct Path *) malloc(sizeof(struct Path));

	if (!path) {
		PERROR_MALLOC;
		return NULL;
	}

	*path = (struct Path) { initial_size, 0, 0 };
	path->trace = (int *) malloc(sizeof(int) * initial_size);

	if (!path->trace) {
		
		PERROR_MALLOC;
		free(path);

		return NULL;
	}

	return path;
}

_static_always_inline
void push_trace(struct Path *path, int *index, int _row, int _col)
{
	if (!path)
		return;
			
	resize_path(path);

	path->trace[*index] = _row;
	path->trace[*index + 1] = _col;

	path->length++;	
	*index += 2;
}

_static_always_inline void copy_trace(struct Path *to, const struct Path *from, int offset)
{
	if (!to || !from)
		return;

	int index = to->length << 1;
	
	for (int i = offset; i < from->length; i++) {
			
		int row = from->trace[i << 1];
		int col = from->trace[(i << 1) + 1];
		
		push_trace(to, &index, row, col);
	}
}

_static_always_inline struct Path *copy_path(const struct Path *path)
{
	if (!path)
		return NULL;

	struct Path *copy = new_path(path->length);

	if (!copy)
		return NULL;

	copy_trace(copy, path, 0);
	copy->time_complexity = path->time_complexity;

	return copy;
}

struct Path *concat_paths(const struct Path *path_a, const struct Path *path_b)
{	
	if (!path_a && !path_b)
		return NULL;
	
	if (!path_a)
		return copy_path(path_b);

	if (!path_b)
		return copy_path(path_a);

	struct Path *concat = new_path(path_a->length + path_b->length);

	if (!concat)
		return NULL;
	
	copy_trace(concat, path_a, 0);
	copy_trace(concat, path_b, 1);

	concat->time_complexity = path_a->time_complexity + path_b->time_complexity;
	return concat;
}

_static_always_inline
int calc_raw_distance(const struct Node *node_a, const struct Node *node_b)
{
	if (!node_a || !node_b)
		return -1;

	return abs(node_b->row - node_a->row) + abs(node_b->col - node_a->col);
}

_static_always_inline
int calc_distance(const struct Node *node_a, const struct Node *node_b)
{
	if (!node_a || !node_b)
		return -1;

	return calc_raw_distance(node_a, node_b) + node_b->span;
}

_static_always_inline
int get_direction(const struct Node *node_a, const struct Node *node_b)
{
	if (!node_a || !node_b)
		return -1;
	
	int row_delta = node_b->row - node_a->row;
	int col_delta = node_b->col - node_a->col;

	if (row_delta < 0)
		return NORTH;

	if (row_delta > 0)
		return SOUTH;

	if (col_delta < 0)
		return WEST;

	if (col_delta > 0)
		return EAST;

	return -1;
}

_static_always_inline void set_priority(struct Node *previous, struct Node *node)
{
	if (!previous || !node)
		return;

	node->priority = calc_distance(previous, node) + previous->priority;
	node->previous = previous;
}

_static_always_inline void reset_priorities(struct Graph *graph)
{
	for (int i = 0; i < graph->rows; i++) {
		for (int j = 0; j < graph->cols; j++) {
			
			if (is_dummy_node(graph->nodes[i][j]))
				continue;

			graph->nodes[i][j].priority = INT_MAX;
			graph->nodes[i][j].previous = NULL;
		}
	}
}

_static_always_inline struct Path *reconstruct_path(struct Node *node)
{
	struct Path *path = new_path(PATH_INITIAL_SIZE);
	
	if (!node || !path)
		return NULL;

	int index = 0;

	struct Node *current = node;
	struct Node *previous = node;

	while (current) {
				
		int distance = calc_raw_distance(previous, current);
		int dir = get_direction(previous, current);

		int row = previous->row;
		int col = previous->col;
		
		for (int i = 0; i < distance; i++) {	
			
			push_trace(path, &index, row, col);
			path->time_complexity++;

			offset_pos(dir, &row, &col);
		}
		
		if (previous->span > 1)
			path->time_complexity++;

		previous = current;
		current = current->previous;
	}
	
	push_trace(path, &index, previous->row, previous->col);
	path->time_complexity += previous->span;

	return path;
}

struct Path *find_shortest_path(struct Graph *graph, int start[2], int dest[2])
{
	struct Heap *heap = new_heap(graph->nodes_count); 

	if (!heap || !graph)
		return NULL;
	
	bool visited[graph->rows * graph->cols];

	memset(visited, 0, sizeof(bool) * graph->rows * graph->cols);

	reset_priorities(graph);
	graph->nodes[start[0]][start[1]].priority = 0;

	insert(heap, &graph->nodes[start[0]][start[1]]);

	struct Node *node = NULL;

	while (!isempty(heap)) {

		node = extract_min(heap);
		
		if (node->row == dest[0] && node->col == dest[1])
			break;
		
		for (int i = 0; i < DIRECTIONS_COUNT; i++) {

			struct Node *neigh = node->neighbours[i];		
			
			if (!neigh || visited[(neigh->row * graph->cols) + neigh->col])
				continue;
			
			int priority = node->priority + calc_distance(node, neigh);

			if (priority < neigh->priority) {	
				set_priority(node, neigh);
				insert(heap, neigh);
			}
		}

		visited[node->row * graph->cols + node->col] = true;
	}
	
	free(heap);
	return reconstruct_path(node);
}
