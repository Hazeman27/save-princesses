#include "min_heap.h"
#include "dijkstra.h"

_static_always_inline
struct Cell make_cell(char span, int row, int col) {
	return (struct Cell) { span, row, col };
}

_static_always_inline
struct Cell extract_cell(const struct Node *node) {
	return (struct Cell) { node->span, node->row, node->col };
}

_static_always_inline
void resize_path(struct Path *path)
{
	if (!path || (path->length << 1) < path->size)
		return;

	path->size <<= 1;
	path->cells = (struct Cell *) realloc(
			path->cells, sizeof(struct Cell) * (path->size));
}

_static_always_inline
struct Path *new_path(int initial_size)
{
	struct Path *path = (struct Path *) malloc(sizeof(struct Path));

	if (!path) {
		PERROR_MALLOC;
		return NULL;
	}

	*path = (struct Path) { initial_size, 0, 0 };
	path->cells = (struct Cell *) malloc(sizeof(struct Cell) * initial_size);

	if (!path->cells) {
		
		PERROR_MALLOC;
		free(path);

		return NULL;
	}

	return path;
}

_static_always_inline
void push_cell(struct Path *path, int *index, struct Cell cell)
{
	if (!path)
		return;
			
	resize_path(path);

	path->cells[*index] = cell;
	path->time_complexity += cell.span;

	path->length++;	
	(*index)++;
}

_static_always_inline
void copy_cells(struct Path *to, const struct Path *from, int offset)
{
	if (!to || !from)
		return;

	int index = to->length;
	
	for (int i = offset; i < from->length; i++)
		push_cell(to, &index, from->cells[i]);
}

_static_always_inline
struct Path *copy_path(const struct Path *path)
{
	if (!path)
		return NULL;

	struct Path *copy = new_path(path->length);

	if (!copy)
		return NULL;

	copy_cells(copy, path, 0);
	return copy;
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

	return calc_raw_distance(node_a, node_b) + node_b->span + node_a->span;
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

_static_always_inline
void set_priority(struct Node *previous, struct Node *node)
{
	if (!previous || !node)
		return;

	node->priority = calc_distance(previous, node) + previous->priority;
	node->previous = previous;
}

_static_always_inline
void reset_priorities(struct Graph *graph)
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

_static_always_inline
struct Path *reconstruct_path(struct Node *node)
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
			
			struct Cell cell;

			if (i > 0)
				cell = make_cell(get_cell_span(ROAD), row, col);
			else cell = extract_cell(previous);

			push_cell(path, &index, cell);
			offset_pos(dir, &row, &col);
		}
		
		previous = current;
		current = current->previous;
	}
	
	push_cell(path, &index, extract_cell(previous));
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
	
	copy_cells(concat, path_a, 0);
	copy_cells(concat, path_b, 1);

	return concat;
}
