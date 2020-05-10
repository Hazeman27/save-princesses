# Save Princesses

> This is a university project at FIIT STU, Bratislava, Slovakia.

## Unforseen Consequences

Resonance cascade at Black Mesa has caused massive damage to the society of the modern
Earth. 7 Hour War brought Combine tyrany upon whole planet. But these events had effect
not only on the future of the humanity, but also on its past.

As multi-dimensional portals were opening around the whole globe, they were causing
tearing between parallel universes. Present and the past were colliding. During this chaos,
Ancient Dragon from foreign galaxy has entered skies of the medieval Europe.
Reigning fear and death, people were certain that this was the end of the world.

Dragon breathed fire upon countries and burned thousands of men, women, and children.
Noone could stop him. But he was not just an animal, self-aware and he
had plans for the Earth. He wanted to control all of the kingdoms and become the sole
Emperor.

His first action towards his plan was to kidnap princesses of the Samhradh royal family.
King Samhradh was devastated, but he wanted his daughters back! So he anounced an enormous
award to anyone who could manage to slay the dragon and return princesses back home. But to
King's disappointment, there was noone willing to risk his life... Except one person.

### Dragonslayer

After few days since kidnapping, a mysterious knight arrived to Samhradh Castle. He
promised to the King that he would slay the beast and bring back all of his daughters.
Hope filled King's heart and he promised that he would double the award if the mysterious
knight would succeed in his mission. So he set horses towards the Dark Mountain, where the
Ancient Dragon had taken Samhradh princesses.

When he arrived to the foot of the mountain, he saw a massive labyrinth of passages through
sharp cliffs. This was the only way to proceed towards the dragon... Before he entered the
labyrinth, he heard a strainge voice behind his back. When he turned, he saw an elderly
man with a covered face. Man spoke:

> "You cannot defeat him when he is awake. You must wait... then
> you shall strike. Make haste, for now his slumber begins..."

## Overview

Save Princesses - is a small program to solve maze like maps with the goal to slay the
dragon, before he wakes up, and save all of the princesses that he has kidnapped.

### Input

Program receives 2 dimensional array of characters that represents the maze, as well
as the wake parameter of the dragon. Set of allowed characters is defined as follows:

| Character | Meaning |
|-----------|---------|
|     D     | Dragon  |
|     P     | Princess|
|     C     | Road    |
|     H     | Bush    |
|     N     | Wall    |

Input can be provided in the form of a file (one map per file), where first line of the
file contains three numbers: `rows`, `cols`, `dragon's wake parameter`. Rest of the file
must contain `rows` amount lines, where each line length equals `cols`. Each character
must be from the set of the allowed characters.

File path can be provided as an argument for the program. For example:

```
$ save_princesses data/map
```

where `data/map` stands for the filepath. When program is called with an argument, it
solves the labyrinth provides an output to the console and exits. If file was not found
or could not be opened, program will fallback to the interaction mode.

### Interaction Mode

This project comes with an interaction tool which launches when no filepath for the
labyrinth is specified as a command line argument, or if opening of the specified file
had failed.

After launching, interaction tool will greet you with the commands list:

| Command                   | Function                     |
|---------------------------|------------------------------|
| h                         | output commands list         |
| f [filepath]              | read labyrinth from the file |
| g [rows] [cols] [drake_t] | generate labyrinth           |
| p                         | print current labyrinth      |
| s                         | save princesses              |
| q                         | exit program                 |


### Data Structures And Algorithms Involved

Project contains implementations of Dijkstra's shortest path algorithm (SPA), min-heap,
and stack. Min-heap and Dijkstra's SPA are used to solve mazes: kill the dragon
and save the princesses before the dragon wakes up. Stack is used for generating maps
for testing purposes.

### Compilation

You can compile program with the provided `Makefile`. It will compile `save_princesses`
program into `out` folder relative to the project directory. Make sure you have `make`
and `gcc` installed on your system, then simply run:

```
$ make
```

If you wish to run provided tests scenarios, you can compile them by providing additional
argument `tests` to the make:

```
$ make tests
```

Tests executable file will be put into the same `out` folder. In an old UNIX fashion, if
everything is ok, running the executable won't produce any output to the console. However,
if you wish to see what functions are being tested and what results are being produced,
you can provide an additional `--verbose` or `-v` argument to the test program:

```
$ out/tests --verbose
```

This will run tests and output information about test functions to the console.

## Implementation

As mentioned above, Dijkstra's SPA and min-heap were used for maze solving.
Dijkstra's algorithm works as BFS (*breadth first search*), but with a priority queue.

### Dijkstra's Shortest Path Algorithm

To solve the maze, it is converted to an undirected graph. All walls and intermediate
roads are skipped.

When looking for the shortest path between two nodes of the graph, we start with an empty
priority queue (heap):

```c
struct Path *find_shortest_path(struct Graph *graph, int start[2], int dest[2])
{
	struct Heap *heap = new_heap(graph->nodes_count);
	...
}
```

We create a boolean array that tells us which nodes we already have visited. Then We set
our start node's priority to `0`, because we are already on that node, and for the rest of
the graph we set all of its nodes priorities to *infinity*, or as in our case, to
`INT_MAX`, which represents that we haven't visited those nodes yet, and we don't know
how far they are:

```c
	bool visited[graph->rows * graph->cols];
	memset(visited, 0, sizeof(bool) * graph->rows * graph->cols);

	reset_priorities(graph);
	graph->nodes[start[0]][start[1]].priority = 0;
```

Then we insert start node into our queue:

```c
	insert(heap, &graph->nodes[start[0]][start[1]]);
```

Finally, we enter the main loop which runs until the queue is empty, or we've found our
destination, in which case we just `break` out of the loop.

Each iteration we extract (*pop*) the minimum value in our queue, which is always on the
top, and check if it is our destination. If yes, then we exit the loop. Otherwise we check
its neighbours, and if they were not visited, we compute their priority relative to to the
current node, and if it is less then their current priority, we update them and insert
them into our queue:

```c
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
```

This algorithm is guaranteed to find the shortest path between two nodes of the graph.

### Time And Space Complexity

After scanning the maze and discovering all of the key nodes (dragon and princesses), we
need to compute the most optimal path from the dragon to each princess. This can be
achieved with a permutation of all possible paths:

```c
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
```

This algorithm takes `O(2 * N * N!)` time, where `N` is the amount target coordinates.
Then, for each of these permutations we find the shortest paths with Dijkstra's algorithm. 

Dijkstra's SPA has time complexity of `O(|V|+|E| * log|V|)` (where `|V|` is the number of
nodes and `|E|` is the number of edges). Combined with our permutations, and initialiazation
time of the Dijkstra's algorithm, this gives us
`O(2 * P * P! + P! * P * ((|V| + |E| * log|V|) + L + M))`, where `P` is the amount of
princesses. `L + M` is the time it takes to concatenate two paths, where `L` is the amount
of cells in the first path, and `M` is the amount of cells in the second path.

Finally, we need to sort our paths and take the shortest one as the main path. For this
purposes we can use quick sort. Quick sort takes `O(N * logN)` comparisons to sort `N`
items, In the worst case, it makes `O(N^2)` comparisons. So if we take the worst case
scenario, then our program will take
`O(2 * P * P! + P! * P * ((|V| + 4 * log|V|) + L + M) + O(N!^2))` time to compute the
best path. `4` is the maximum amount of edges that node may have in the *Manhattan* like
paths, which is exactly the case with our maze.

Each node of the graph has a structure:

```c
struct Node {
	char span;
	int row;
	int col;
	int priority;
	struct Node *previous;
	struct Node *neighbours[DIRECTIONS_COUNT]; 
};
```

where `DIRECTIONS_COUNT` equals `4`. This structure has size of `56` bytes.

To reduce amount of generated nodes, intermediate roads are skipped. For example, map:

```
CCCCDCCCP
```

will produce only 3 nodes: starting node, dragon node, princesses node.

### Map Generation

Map generation is realized through simple algorithm with stack. Simply put, we just walk
entire map by pushing each coordinate onto stack. If we encounter a dead end, we can easily
backtrack by popping coordinates from the stack. There isn't really much to stay about
the logic of this algorithm, more challenging part (*at least for me*) is to create walls
between paths. The most important bit is this `while` loop:

```c
	while (peek_top(stack)) {
		
		int dir = find_direction(map, visited, row, col);
		
		if (dir < 0) {
			if (pop_pair(stack, &col, &row) < 0)
				goto stack_failure;
			continue;
		}
		
		add_walls(map, dir, row, col);
		
		if (push_direction(stack, visited, dir, &row, &col) < 0)
			goto stack_failure;
	}
```

Similar to searching paths, we keep track of the visited nodes. We keep traversing the
map until we backtrack all the way to the beginning of the map, i.e. the stack is empty.
So in that fashion, this loop is a bit similar to Dijkstra's SPA main loop.

## Testing

Testing was realized through custom and random generated maps. Custom maps were taken
from this [source](https://drive.google.com/drive/folders/1aPOqsTCRHPG48I17CX5rIJt0-sus9-An).

Here are some testing results:

| Map size | Princesses count | Distance to dragon | Time to slay dragon  | Time to save princesses| Path time complexity |
|----------|------------------|--------------------|----------------------|------------------------|----------------------|
| 10 x 10  | 1                | 23				   | ~20000 ns            | ~8000 ns               | 53                   |
| 20 x 20  | 1                | 16                 | ~12000 ns            | ~35700 ns			   | 39                   |
| 50 x 50  | 1                | 76                 | ~232000 ns           | ~250000 ns             | 148                  |
| 100 x 100| 1                | 117                | ~360000 ns           | ~315000 ns             | 238                  |
| 20 x 20  | 2                | 20                 | ~17700 ns            | ~67700 ns			   | 29                   |
| 50 x 50  | 2                | 3                  | ~47400 ns            | ~551300 ns             | 131                  |
| 500 x 500| 2                | 644                | ~17508500 ns         | ~60907400 ns           | 1763                 |
| 500 x 500| 3                | 580                | ~13010900 ns         | ~230075800 ns          | 1558                 |
| 10 x 10  | 4                | 10                 | ~13000 ns            | ~360000 ns             | 40                   |
| 50 x 50  | 4                | 100                | ~300000 ns           | ~8630000 ns            | 319                  |
| 100 x 100| 4                | 154                | ~745000 ns           | ~41000000 ns           | 545                  |
| 500 x 500| 4                | 275                | ~5032400 ns          | ~61251200 ns           | 1852                 |
| 10 x 10  | 5                | 1                  | ~2000 ns             | ~1660000 ns            | 56                   |
| 20 x 20  | 5                | 21                 | ~13100 ns            | ~7561700 ns            | 142                  |
| 100 x 100| 5                | 13                 | ~170000 ns           | ~262000000 ns          | 576                  |

Nizomiddin Toshpulatov (tremul27@gmail.com)
FIIT STU, Bratislava, 2020
