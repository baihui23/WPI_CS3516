#include <stdio.h>
#include <stdlib.h>
#include "project3.h"

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};

// Function prototypes
int compute_min_for_column(int column, struct distance_table *distanceTable);