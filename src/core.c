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

_static_always_inline void swap_pos(int pos_a[2], int pos_b[2])
{
	int tmp_x = pos_a[0];
	int tmp_y = pos_a[1];
	
	pos_a[0] = pos_b[0];
	pos_a[1] = pos_b[1];

	pos_b[0] = tmp_x;
	pos_b[1] = tmp_y;
}

_static_always_inline int compare_paths(const void *path_a, const void *path_b)
{
	return ((struct Path *) path_a)->time_complexity -
		((struct Path *) path_b)->time_complexity;
}

_static_always_inline int factorial(int n)
{
	if (n < 0) {
		errno = EINVAL;
		return -1;
	}

	int result = 1;

	for (int i = n; i > 1; i--)
		result *= i;
	
	return result;
}

static void permute(int targets[][2], int start, int targetsc, int results[][2], int *offset) 
{ 
	if (start == targetsc) {
		
		for (int i = 0; i < targetsc; i++)
			copy_pos(results[*offset + i], targets[i]);
		
		*offset += targetsc;
		return;
	}

	for (int i = start; i < targetsc; i++) { 
		
		swap_pos(targets[start], targets[i]); 
		permute(targets, start + 1, targetsc, results, offset);

		swap_pos(targets[start], targets[i]); 
	} 
}

static struct Path *permute_princesses_paths(struct Rescue_Mission *mission)
{
	if (!mission)
		return NULL;

	int permutations_count = factorial(mission->princesses_count);
	int princesses_count = mission->princesses_count;

	int permutations[permutations_count * princesses_count][2];

	struct Path *paths[permutations_count];
	int offset = 0;
	
	permute(mission->princesses_pos, 0, princesses_count, permutations, &offset);
		
	for (int i = 0; i < permutations_count; i++) {
		
		paths[i] = find_shortest_path(
				mission->graph,
				mission->drake_pos,
				permutations[i * princesses_count]);

		for (int j = 0; j < princesses_count - 1; j++) {
			
			struct Path *path = find_shortest_path(
					mission->graph,
					permutations[i * princesses_count + j],
					permutations[i * princesses_count + j + 1]);
			
			struct Path *concat = concat_paths(path, paths[i]);

			free_path(paths[i]);
			free_path(path);

			paths[i] = concat;
		}
	}

	qsort(paths, permutations_count, sizeof(struct Path *), compare_paths);
	return paths[0];
}

int *save_princesses(struct Map *map, int *path_length, bool reverse_axis)
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

	struct Path *to_princesses = permute_princesses_paths(mission);	
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

	printf("Time complexity of the path: %d\n", final->time_complexity);

	*path_length = final->length;
	int *trace = extract_trace(final, reverse_axis);

	free(final);
	return trace;

mission_failed:
	*path_length = 0;	
	return NULL;
}

int *zachran_princezne(char **mapa, int n, int m, int t, int *dlzka_cesty) {
	
	struct Map *map = new_map(n, m, t);
	
	if (!map) {
		*dlzka_cesty = 0;
		return NULL;
	}

	for (int i = 0; i < n; i++)
		memmove(map->cells, mapa, sizeof(char *) * m);

	int *trace = save_princesses(map, dlzka_cesty, true);
	free_map(map);

	return trace;
}
