#include <stdio.h>
#include <stdlib.h>
#include "project3.h"

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};

// Function prototypes
void init_to_infinity(struct distance_table *distanceTable);
void set_direct_adjacent_costs( struct NeighborCosts *neighborCosts, struct distance_table *distanceTable);
void send_to_neighbors(int source_id, int numNodesInNetwork, struct distance_table *distanceTable);
int compute_min_for_column(int column, struct distance_table *distanceTable);
