#include "node_helper.h"

int compute_min_for_column(int column, struct distance_table *distanceTable){
    int i;
    int min = INFINITY;

    for(i = 0; i < MAX_NODES; i++)
    {
        int temp = distanceTable->costs[i][column];
        min = temp < min ? temp : min;
    }

    return min;
}