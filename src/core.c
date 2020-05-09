#include "core_augmented.h"
#include "rescue_mission.h"

int *save_princesses(struct Map *map, int *path_length)
{
	if (!map)
		return NULL;	
	
	struct Rescue_Mission *mission = make_rescue_mission(map);
	print_rescue_mission(mission);
	
	free_rescue_mission(mission);
	return NULL;
}
