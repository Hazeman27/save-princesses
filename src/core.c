#include "core_augmented.h"
#include "core_utils.h"
#include "rescue_mission.h"
#include "min_heap.h"

#define PATH_INITIAL_SIZE 32

struct _path {
	int size;
	int length;
	int time_complexity;
	int *trace;
};

_static_always_inline void resize_path(struct _path *path)
{
	if ((path->length << 1) < path->size)
		return;

	path->size <<= 1;
	path->trace = (int *) realloc(path->trace, sizeof(int) * (path->size));
}

_static_always_inline struct _path *_new_path(int initial_size)
{
	struct _path *path = (struct _path *) malloc(sizeof(struct _path));

	if (!path) {
		PERROR_MALLOC;
		return NULL;
	}

	*path = (struct _path) { initial_size, 0, 0 };
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

	return abs(node_b->row - node_a->row) +
		abs(node_b->col - node_a->col) + node_b->span;
}

_static_always_inline
int get_direction(const struct Node *node_a, const struct Node *node_b)
{
	if (!node_a || !node_b)
		return -1;

	if ((node_b->row - node_a->row) < 0)
		return NORTH;

	if ((node_b->row - node_a->row) > 0)
		return SOUTH;

	if ((node_b->col - node_a->col) < 0)
		return WEST;

	if ((node_b->col - node_a->col) > 0)
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

_static_always_inline void free_visited(bool *visited[], int rows)
{
	for (int i = 0; i < rows; i++)
		free(visited[i]);
}

_static_always_inline bool alloc_visited(bool *visited[], int rows, int cols)
{
	for (int i = 0; i < rows; i++) {
		visited[i] = (bool *) calloc(cols, sizeof(bool));

		if (!visited[i]) {
			PERROR_MALLOC;
			return false;
		}
	}

	return true;
}

_static_always_inline struct _path *reconstruct_path(struct Node *node)
{
	struct _path *path = _new_path(PATH_INITIAL_SIZE);
	
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

_static_always_inline
void set_priorities(struct Graph *graph, struct Heap *heap, int start[2])
{
	if (!graph || !heap)
		return;	
	
	for (register int row = 0; row < graph->rows; row++) {
		
		for (register int col = 0; col < graph->cols; col++) {
			
			if (is_dummy_node(graph->nodes[row][col]))
				continue;
			
			if (row == start[0] && col == start[1])
				graph->nodes[row][col].priority = 0;
			
			else graph->nodes[row][col].priority = INT_MAX;

			graph->nodes[row][col].previous = NULL;

			insert(heap, &graph->nodes[row][col]);
		}
	}
}

static struct _path *find_shortest_path(struct Graph *graph, int start[2], int dest[2])
{
	struct Heap *heap = new_heap(graph->nodes_count); 
	
	if (!heap || !graph)
		return NULL;
	
	set_priorities(graph, heap, start);

	int i;
	struct Node *node = NULL;
	struct Node *neighbour = NULL;

	bool *visited[graph->rows];
	
	if (!alloc_visited(visited, graph->rows, graph->cols))
		return NULL;

	while (!isempty(heap)) {

		node = extract_min(heap);
			
		if (node->row == dest[0] && node->col == dest[1])
			break;
		
		for (i = 0; i < DIRECTIONS_COUNT; i++) {

			neighbour = node->neighbours[i];		
			
			if (!neighbour || visited[neighbour->row][neighbour->col])
				continue;
			
			int priority = node->priority + calc_distance(node, neighbour);

			if (priority < neighbour->priority) {	
				set_priority(node, neighbour);
				min_heapify(heap);
			}
		}

		visited[node->row][node->col] = true;
	}
	
	free(heap);
	free_visited(visited, graph->rows);
	
	return reconstruct_path(node);
}

void print_path_on_map(struct Map *map, struct _path *path)
{
	if (!map || !path)
		return;
	
	struct Map *copy = copy_map(map);

	if (!copy)
		return;
	
	printf(">> Path:\n\tLength: %d\n\tTime complexity: %d\n",
			path->length, path->time_complexity);

	for (int i = 0; i < path->length; i++)
		copy->cells[path->trace[(i << 1)]][path->trace[(i << 1) + 1]] = PATH;

	print_map(copy);
	free_map(copy);
}

int *save_princesses(struct Map *map, int *path_length)
{
	if (!map)
		return NULL;	
		
	struct Rescue_Mission *mission = make_rescue_mission(map);
	// print_rescue_mission(mission);
	
	int start[2] = { 0, 0 };
	struct _path *to_drake = find_shortest_path(
			mission->graph, start, mission->drake_pos);
	
	if (!to_drake) {
		printf(DRAKE_PATH_NOT_FOUND);
		*path_length = 0;
		return NULL;
	}
	
	struct _path *to_princesses[mission->princesses_count];

	for (int i = 0; i < mission->princesses_count; i++) {
		
		to_princesses[i] = find_shortest_path(
				mission->graph,
				mission->drake_pos,
				mission->princesses_pos[i]);

		if (!to_princesses[i]) {
			printf(PRINCESS_PATH_NOT_FOUND);
			*path_length = 0;
			return NULL;
		}

		print_path_on_map(map, to_princesses[i]);
	}

	free_rescue_mission(mission);
	return NULL;
}
