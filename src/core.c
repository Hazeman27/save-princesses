#include "core_augmented.h"
#include "core_utils.h"
#include "rescue_mission.h"
#include "min_heap.h"

_static_always_inline
int calc_distance(const struct Node *node_a, const struct Node *node_b)
{
	if (!node_a || !node_b)
		return -1;

	return abs(node_b->row - node_a->row) + abs(node_b->col - node_a->col);
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

_static_always_inline int *resize_path(int *path, int *size, int length)
{
	if ((length << 1) < *size)
		return path;

	*size <<= 1;
	return (int *) realloc(path, sizeof(int) * (*size));
}

_static_always_inline int *reconstruct_path(struct Node *node, int *path_length)
{
	int path_size = 32;
	int *path = (int *) malloc(sizeof(int) * path_size);
	
	if (!node || !path)
		return NULL;
	
	int row = 0;
	int col = 1;

	struct Node *current = node;
	struct Node *previous = node;

	while (current) {
				
		int distance = calc_distance(previous, current);
		int dir = get_direction(previous, current);

		int _row = previous->row;
		int _col = previous->col;

		for (int i = 0; i < distance; i++) {	
			
			resize_path(path, &path_size, *path_length);

			path[row] = _row;
			path[col] = _col;

			row += 2;
			col += 2;
			
			(*path_length)++;
			offset_pos(dir, &_row, &_col);
		}
		
		previous = current;
		current = current->previous;
	}

	return path;
}

static int *find_shortest_path(struct Graph *graph, int start[2], int dest[2], int *path_length)
{
	struct Heap *heap = new_heap(graph->nodes_count); 
	
	if (!heap)
		return NULL;

	graph->nodes[start[0]][start[1]].priority = 0;
	
	for (register int row = 0; row < graph->rows; row++) {
		for (register int col = 0; col < graph->cols; col++) {
			if (!is_dummy_node(graph->nodes[row][col]))
				insert(heap, &graph->nodes[row][col]);
		}
	}
	
	int i;
	struct Node *node = NULL;
	struct Node *neighbour = NULL;

	bool *visited[graph->rows];

	for (i = 0; i < graph->rows; i++) {
		
		visited[i] = (bool *) calloc(graph->cols, sizeof(bool));

		if (!visited[i]) {
			PERROR_MALLOC;
			return NULL;
		}
	}

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
	return reconstruct_path(node, path_length);
}

void print_path_on_map(struct Map *map, int *path, int *path_length)
{
	if (!map || !path || *path_length < 0)
		return;
	
	struct Map *copy = copy_map(map);

	if (!copy)
		return;

	for (int i = 0; i < *path_length; i++)
		copy->cells[path[(i << 1)]][path[(i << 1) + 1]] = PATH;

	print_map(copy);
	free_map(copy);
}

int *save_princesses(struct Map *map, int *path_length)
{
	if (!map)
		return NULL;	
	
	*path_length = 0;	
	
	struct Rescue_Mission *mission = make_rescue_mission(map);
	//print_rescue_mission(mission);
	
	int start[2] = { 0, 0 };

	int *path_to_drake = find_shortest_path(
			mission->graph, start, mission->drake_pos, path_length);

	if (path_to_drake)
		print_path_on_map(map, path_to_drake, path_length);

	free_rescue_mission(mission);
	return NULL;
}
