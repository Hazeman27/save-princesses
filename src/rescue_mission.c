#include "core_utils.h"
#include "rescue_mission.h"

_static_always_inline bool is_wall(struct Map *map, int row, int col) {
	return map->cells[row][col] == WALL;
}

_static_always_inline bool is_bush(struct Map *map, int row, int col) {
	return map->cells[row][col] == BUSH;
}

_static_always_inline bool is_drake(struct Map *map, int row, int col) {
	return map->cells[row][col] == DRAKE;
}

_static_always_inline bool is_princess(struct Map *map, int row, int col) {
	return map->cells[row][col] == PRINCESS;
}

_static_always_inline bool is_junction(struct Map *map, int row, int col)
{
	if (out_of_bounds(map, NORTH, row, col) && out_of_bounds(map, SOUTH, row, col))
		return false;
	
	if (out_of_bounds(map, turn_clockwise(NORTH), row, col) &&
		       out_of_bounds(map, turn_counter_clockwise(NORTH), row, col))
		return false;
	
	if (out_of_bounds(map, turn_clockwise(SOUTH), row, col) &&
		       out_of_bounds(map, turn_counter_clockwise(SOUTH), row, col))
		return false;
	
	return true;
}

_static_always_inline bool is_node_cell(struct Map *map, int row, int col)
{
	return (!row && !col)	||
		is_junction(map, row, col) 	||
		is_bush(map, row, col) 		||
		is_princess(map, row, col)	||
		is_drake(map, row, col);
}

_static_always_inline void set_pos(int pos[2], int row, int col) {
	pos[0] = row, pos[1] = col;
}

_static_always_inline void free_graph(struct Graph *graph)
{
	if (!graph)
		return;

	for (size_t i = 0; i < graph->rows; i++)
		free(graph->nodes[i]);

	free(graph);
}

_static_always_inline struct Graph *new_graph(int rows, int cols)
{
	struct Graph *graph = (struct Graph *) malloc(
			sizeof(struct Graph) + sizeof(struct Node *) * rows);

	if (!graph) {
		PERROR_MALLOC;
		return NULL;
	}

	*graph = (struct Graph) { rows, cols, 0 };

	for (int i = 0; i < rows; i++) {
		
		graph->nodes[i] = (struct Node *) malloc(sizeof(struct Node) * cols);

		if (!graph->nodes[i]) {
			
			PERROR_MALLOC;
			free_graph(graph);

			return NULL;
		}

		for (int j = 0; j < cols; j++) {
			graph->nodes[i][j] = (struct Node) {
				DUMMY_NODE_SPAN,
				DUMMY_NODE_ROW,
				DUMMY_NODE_COL
			};
		}
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
	
	graph->nodes_count++;
	graph->nodes[row][col] = (struct Node) {
		get_cell_span(map->cells[row][col]), row, col, INT_MAX
	};
	
	for (int dir = 0; dir < DIRECTIONS_COUNT; dir++) {
		
		if (out_of_bounds(map, dir, row, col))
			continue;
		
		_row = row;
		_col = col;

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

void free_rescue_mission(struct Rescue_Mission *mission)
{
	if (!mission)
		return;

	free_graph(mission->graph);
	free(mission);
}

struct Rescue_Mission *make_rescue_mission(struct Map *map)
{
	if (!map)
		return NULL;
	
	struct Graph *graph = new_graph(map->rows, map->cols);
	
	int princesses_count = 0;
	int drake_pos[2];
	int princesses_pos[PRINCESSES_MAX_COUNT][2];

	if (!graph)
		return NULL;

	for (register int row = 0; row < map->rows; row++) {

		for (register int col = 0; col < map->cols; col++) {
			
			if (is_wall(map, row, col) || !is_node_cell(map, row, col))
				continue;		
			
			if (is_drake(map, row, col))
				set_pos(drake_pos, row, col);
				
			else if (is_princess(map, row, col)) {
				set_pos(princesses_pos[princesses_count], row, col);
				princesses_count++;
			}
				
			scan_node(graph, map, row, col);
		}
	}
	
	return new_rescue_mission(graph, princesses_count, drake_pos, princesses_pos);
}

void print_graph_nodes(const struct Graph *graph)
{
	if (!graph)
		return;

	for (int i = 0; i < graph->rows; i++) {
		for (int j = 0; j < graph->cols; j++) {
			
			if (is_dummy_node(graph->nodes[i][j]))
				continue;
			
			struct Node node = graph->nodes[i][j];

			printf("Node[%d, %d]: ", node.row, node.col);

			for (int k = 0; k < DIRECTIONS_COUNT; k++) {
				
				if (!node.neighbours[k])
					continue;

				printf("(%d, %d) ",
						node.neighbours[k]->row,
						node.neighbours[k]->col);
			}

			printf("\n");
		}
	}
}

void print_graph(const struct Graph *graph)
{
	if (!graph)
		return;
	
	char *prefix;
	char *suffix;

	bool pretty_print = true;

	if (graph->cols > 25) {
		pretty_print = false;
		prefix = "";
		suffix = "";
	} else {
		prefix = " ";
		suffix = " ";
	}
	
	if (pretty_print) {
		
		printf("   ");
		
		for (int i = 0; i < graph->cols; i++)
			printf(CLR_YELLOW "%02d " RESET, i);
		
		printf("\n");
	}
	
	for (int i = 0; i < graph->rows; i++) {
		
		if (pretty_print)
			printf(CLR_YELLOW "%02d " RESET, i);
		
		for (int j = 0; j < graph->cols; j++) {	
			
			int span = graph->nodes[i][j].span;

			if (is_dummy_node(graph->nodes[i][j]))
				printf("%s-%s", prefix, suffix);

			else {
				switch (span) {
					case 1: printf(CLR_YELLOW); break;
					case 2: printf(CLR_RED); break;
					default: break;
				}
				
				printf("%s%d%s" RESET, prefix, span, suffix);
			}
		}
		
		printf("\n");
	}
}

void print_rescue_mission(struct Rescue_Mission *mission)
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
