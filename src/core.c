#include "core_augmented.h"
#include "map_generator_augmented.h"

#define DUMMY_NODE_SPAN -1

struct Node {
	char span;
	struct Node *neighbours[DIRECTIONS_COUNT]; 
};

struct Graph {
	int rows;
	int cols;
	struct Node *nodes[];
};

struct Rescue_Mission {
	struct Graph *graph;
	int princesses_count;
	int drake_pos[2];
	int princesses_pos[][2];	
};

_static_always_inline bool is_dummy_node(struct Node node) {
	return node.span == DUMMY_NODE_SPAN;
}

_static_always_inline void free_graph(struct Graph *graph)
{
	if (!graph)
		return;

	for (size_t i = 0; i < graph->rows; i++)
		free(graph->nodes[i]);

	free(graph);
}

_static_always_inline void free_rescue_mission(struct Rescue_Mission *mission)
{
	if (!mission)
		return;

	free_graph(mission->graph);
	free(mission);
}

_static_always_inline struct Node *new_node(char span)
{
	struct Node *node = (struct Node *) malloc(sizeof(struct Node));

	if (!node) {
		PERROR_MALLOC;
		return NULL;
	}

	node->span = span;
	return node;
}

_static_always_inline struct Graph *new_graph(int rows, int cols)
{
	struct Graph *graph = (struct Graph *) malloc(
			sizeof(struct Graph) + sizeof(struct Node *) * rows);

	if (!graph) {
		PERROR_MALLOC;
		return NULL;
	}

	*graph = (struct Graph) { rows, cols };

	for (int i = 0; i < rows; i++) {
		
		graph->nodes[i] = (struct Node *) malloc(sizeof(struct Node) * cols);

		if (!graph->nodes[i]) {
			
			PERROR_MALLOC;
			free_graph(graph);

			return NULL;
		}

		for (int j = 0; j < cols; j++)
			graph->nodes[i][j].span = DUMMY_NODE_SPAN;
	}

	return graph;
}

_static_always_inline struct Rescue_Mission *new_rescue_mission(
		struct Graph *graph,
		int princesses_count,
		int drake_pos[2],
		int princesses_pos[][2])
{
	if (!graph)
		return NULL;

	struct Rescue_Mission *mission = (struct Rescue_Mission *) malloc(
			sizeof(struct Rescue_Mission) +
			sizeof(int) * (princesses_count << 1));

	if (!mission) {
		PERROR_MALLOC;
		return NULL;
	}

	*mission = (struct Rescue_Mission) { graph, princesses_count };
	
	memcpy(mission->drake_pos, drake_pos,
			(sizeof(int) << 1));

	memcpy(mission->princesses_pos, princesses_pos,
			sizeof(int) * (princesses_count << 1));

	return mission;
}

_static_always_inline
void connect_nodes(struct Node *node_a, struct Node *node_b, int dir)
{
	if (!node_a || !node_b)
		return;

	node_a->neighbours[dir] = node_b;
	node_b->neighbours[reverse_direction(dir)] = node_a;
}

_static_always_inline
void scan_node(struct Graph *graph, struct Map *map, int row, int col)
{
	int _row;
	int _col;
	
	graph->nodes[row][col] = (struct Node) {
		get_cell_span(map->cells[row][col])
	};
	
	for (int dir = 0; dir < DIRECTIONS_COUNT; dir++) {
		
		if (out_of_bounds(map, dir, row, col))
			continue;

		do {
			if (offset_pos(dir, &_row, &_col) < 0)
				return;
			
			if (is_dummy_node(graph->nodes[_row][_col]))
				continue;

			connect_nodes(&graph->nodes[row][col], &graph->nodes[_row][_col], dir);
			break;
		} while (!out_of_bounds(map, dir, _row, _col));
	}
}

_static_always_inline bool is_node_cell(struct Map *map, int row, int col)
{
	return !out_of_bounds(map, NORTH, row, col)
		|| !out_of_bounds(map, SOUTH, row, col)
		|| map->cells[row][col] == BUSH
		|| map->cells[row][col] == PRINCESS
		|| map->cells[row][col] == DRAKE;
}

_static_always_inline struct Rescue_Mission *make_rescue_mission(struct Map *map)
{
	if (!map)
		return NULL;
	
	struct Graph *graph = new_graph(map->rows, map->cols);
	
	int princesses_count = 0;
	int drake_pos[2];
	int princesses_pos[PRINCESSES_MAX_COUNT][2];

	if (!graph)
		return NULL;

	for (int row = 0; row < map->rows; row++) {

		for (int col = 0; col < map->cols; col++) {
			
			if (map->cells[row][col] == WALL || !is_node_cell(map, row, col))
				continue;		
			
			if (map->cells[row][col] == DRAKE) {
				drake_pos[0] = row;
				drake_pos[1] = col;
			}
				
			else if (map->cells[row][col] == PRINCESS) {

				princesses_pos[princesses_count][0] = row;
				princesses_pos[princesses_count][1] = col;

				princesses_count++;
			}
				
			scan_node(graph, map, row, col);
		}
	}
	
	return new_rescue_mission(graph, princesses_count, drake_pos, princesses_pos);
}

_static_always_inline void print_graph(struct Graph *graph)
{
	if (!graph)
		return;

	for (int row = 0; row < graph->rows; row++) {
		
		for (int col = 0; col < graph->cols; col++) {
			
			int span = graph->nodes[row][col].span;

			if (is_dummy_node(graph->nodes[row][col]))
				putchar(WALL);

			else {
				switch (span) {
					case 1: printf(CLR_YELLOW); break;
					case 2: printf(CLR_RED); break;
					default: break;
				}
				
				printf("%d" CLR_RESET, span);
			}
		}

		putchar('\n');
	}
}

_static_always_inline void print_rescue_mission(struct Rescue_Mission *mission)
{
	if (!mission)
		return;

	printf(">> Rescue Mission:\n");
	printf("Drake's location: [%d, %d]\n", mission->drake_pos[0], mission->drake_pos[1]);
	printf("Princesses count: %d\n", mission->princesses_count);
	printf("Princesses locations:\n");

	for (int i = 0; i < mission->princesses_count; i++) {
		printf("\t#P%d: [%d, %d]\n", i,
				mission->princesses_pos[i][0], 
				mission->princesses_pos[i][1]); 
	}
	
	printf("Mission graph:\n");
	print_graph(mission->graph);
}

int *save_princesses(struct Map *map, int *path_length)
{
	if (!map)
		return NULL;	
	
	struct Rescue_Mission *mission = make_rescue_mission(map);
	print_rescue_mission(mission);

	return NULL;
}
