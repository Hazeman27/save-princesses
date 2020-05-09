#include "core_utils.h"
#include "min_heap.h"
#include "dijkstra.h"

_static_always_inline void resize_path(struct Path *path)
{
	if ((path->length << 1) < path->size)
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
	for (register int i = 0; i < graph->rows; i++) {
		for (register int j = 0; j < graph->cols; j++) {
			
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

	int row = 0;
	int col = 1;

	struct Node *current = node;
	struct Node *previous = node;

	while (current) {
				
		int distance = calc_raw_distance(previous, current);
		int dir = get_direction(previous, current);

		int _row = previous->row;
		int _col = previous->col;
		
		for (int i = 0; i < distance; i++) {	
			
			resize_path(path);

			path->trace[row] = _row;
			path->trace[col] = _col;

			row += 2;
			col += 2;
			
			path->length++;
			path->time_complexity++;

			offset_pos(dir, &_row, &_col);
		}
		
		if (previous->span > 1)
			path->time_complexity++;

		previous = current;
		current = current->previous;
	}

	resize_path(path);

	path->trace[row] = previous->row;
	path->trace[col] = previous->col;

	path->length++;
	path->time_complexity += previous->span;

	return path;
}

struct Path *find_shortest_path(struct Graph *graph, int start[2], int dest[2])
{
	struct Heap *heap = new_heap(graph->nodes_count); 

	if (!heap || !graph)
		return NULL;
	
	bool *visited = (bool *) calloc(graph->rows * graph->cols, sizeof(bool));

	if (!visited) {
		PERROR_MALLOC;
		return NULL;
	}

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
	free(visited);
	return reconstruct_path(node);
}

void print_path(struct Map *map, struct Path *path)
{
	if (!map || !path)
		return;
	
	struct Map *copy = copy_map(map);

	if (!copy)
		return;
	
	printf(">> Path:\n\tLength: %d\n\tTime complexity: %d\n",
			path->length, path->time_complexity);

	for (int i = 0; i < path->length; i++) {

		int row = path->trace[(i << 1)];
		int col = path->trace[(i << 1) + 1];
		
		if (copy->cells[row][col] != DRAKE && copy->cells[row][col] != PRINCESS)
			copy->cells[row][col] = PATH;
	}

	print_map(copy);
	free_map(copy);
}
