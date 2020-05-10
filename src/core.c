#include "core_augmented.h"
#include "core_utils.h"
#include "dijkstra.h"

#define ORIGIN_ROW 0
#define ORIGIN_COL 0

_static_always_inline void copy_pos(int to[2], int from[2]) {
	to[0] = from[0], to[1] = from[1];
}

_static_always_inline void set_pos(int pos[2], int row, int col) {
	pos[0] = row, pos[1] = col;
}

_static_always_inline int compare_paths(const void *path_a, const void *path_b)
{
	if (!path_a || !path_b)
		return -1;

	return ((struct Path *) path_a)->time_complexity -
		((struct Path *) path_b)->time_complexity;
}


static struct Path *permute_paths(struct Graph *graph, int origin[2], int targets[][2], int targets_count)
{
	if (!graph)
		return NULL;
	
	struct Path *path = NULL;
	struct Path *paths[targets_count];
	
	paths[0] = NULL;

	for (int i = 0; i < targets_count; i++) {
		
		int new_targets[targets_count - 1][2];

		for (int j = 0, k = 0; j < targets_count; j++) {

			if (j == i)
				continue;

			copy_pos(new_targets[k++], targets[j]);
		}

		path = permute_paths(graph, targets[i], new_targets, targets_count - 1);	
		
		paths[i] = concat_paths(
				path, find_shortest_path(graph, origin, targets[i]));
	}
	
	qsort(paths, targets_count, sizeof(struct Path *), compare_paths);
	return paths[0];
}

int *save_princesses(struct Map *map, int *path_length)
{
	if (!map)
		goto mission_failed;	
	
	struct timespec start;
	struct timespec end;
	
	record_timestamp(&start);
	struct Rescue_Mission *mission = make_rescue_mission(map);

	record_timestamp(&end);
	print_delta_time(stdout, "Mission briefing", calc_delta_time(start, end)); 

	if (!mission)
		goto mission_failed;

	print_rescue_mission(mission);
	int origin[2] = { ORIGIN_ROW, ORIGIN_COL };
	
	record_timestamp(&start);

	struct Path *to_drake = find_shortest_path(
			mission->graph, origin, mission->drake_pos);
	
	record_timestamp(&end);

	if (!to_drake) {

		printf(DRAKE_PATH_NOT_FOUND);
		free_rescue_mission(mission);
		
		goto mission_failed;
	}

	if (to_drake->time_complexity > map->drake_wake_time) {	
		
		printf(DRAKE_AWAKEN);

		free_path(to_drake);
		free_rescue_mission(mission);
		
		goto mission_failed;
	}

	print_delta_time(stdout, "Drake elimination", calc_delta_time(start, end));
	record_timestamp(&start);

	struct Path *to_princesses = permute_paths(
			mission->graph,
			mission->drake_pos,
			mission->princesses_pos,
			mission->princesses_count);
	
	record_timestamp(&end);

	if (!to_princesses) {
		
		printf(PRINCESS_PATH_NOT_FOUND);

		free_path(to_drake);
		free_rescue_mission(mission);
		
		goto mission_failed;
	}
	
	print_delta_time(stdout, "Princesses rescuing", calc_delta_time(start, end));

	struct Path *final = concat_paths(to_princesses, to_drake);
	
	free_rescue_mission(mission);
	free_path(to_drake);
	free_path(to_princesses);
	
	if (!final)
		goto mission_failed;

	*path_length = final->length;
	return reverse_trace(final->trace, final->length, false);

mission_failed:
	*path_length = 0;	
	return NULL;
}
