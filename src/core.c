#include "core_augmented.h"
#include "dijkstra.h"

int *save_princesses(struct Map *map, int *path_length)
{
	if (!map)
		return NULL;	
		
	struct Rescue_Mission *mission = make_rescue_mission(map);
	print_rescue_mission(mission);
	
	int start[2] = { 0, 0 };
	struct Path *to_drake = find_shortest_path(
			mission->graph, start, mission->drake_pos);
	
	if (!to_drake) {
		printf(DRAKE_PATH_NOT_FOUND);
		*path_length = 0;
		return NULL;
	}
	
	struct Path *to_princesses[mission->princesses_count];

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

		print_path(map, to_princesses[i]);
	}

	free_rescue_mission(mission);
	return NULL;
}
