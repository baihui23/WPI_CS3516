#include "node_helper.h"

extern int TraceLevel;
extern float clocktime;

/*
 * Prints the method called being passed as string parameter
 *@param methodCalled The method that is called and that needs to be traced
 */
void print_trace(char *methodCalled){
    // If there is a high enough level for tracing pring the clock time when this is called
    if (TraceLevel >= 0){
        printf("At time t=%f, %s called.\n", clocktime, methodCalled);
    }
}

/*
 * Sets all the cost in the given distance table a value of infinity
 * @param distanceTable The pointer to the distance table whose nodes' cost need to be set to infinity
 */
void init_to_infinity(struct distance_table *distanceTable){
    int row;                                                                    // row loop counter
    int column;                                                                 // column loop counter

    // Loop (in both direction) and assign infinity
    for(row = 0; row < MAX_NODES; row++){
        for(column = 0; column < MAX_NODES; column++){
            distanceTable->costs[row][column] = INFINITY;
        }
    }   
}

/*
 * Sets the cost of the nodes directly connected
 * @param neighborCosts The pointer to the struct that contains costs of pahts
 * @param distanceTable The pointer to the distance table that contains distances that will be updated
 */
void set_direct_adjacent_costs(struct NeighborCosts *ncs, struct distance_table *distanceTable){
    int i;                                                                          // Loop counter

    // Loop and assign adjacent costs
    for(i = 0; i < ncs->NodesInNetwork; i++){
        distanceTable->costs[i][i] = ncs->NodeCosts[i];
    }
}

/*
 * This method creates a packet for each node in the network then computes the min cost of for each colum in
 * the distance table and one by one sends them to neighbors
 * @param source_id The id of the node send to its neighbors
 * @param numNodesInNetwork The number of nodes in the network
 * @param distanceTable The pointer to the distance table that contains cost values
 */
void send_to_neighbors(int source_id, int numNodesInNetwork, struct distance_table *distanceTable){
    int i;                                                                      // Loop counter

    // Loop over each node in the network
    for(i = 0; i < numNodesInNetwork; i++){
        // Make sure the current node is not the node that will be sending the packet
        if(i != source_id){
            // If it's not then create the packet
            struct RoutePacket *routePacket = (struct RoutePacket *)malloc(sizeof(struct RoutePacket));
            routePacket->destid = i;                                                    // Set the destination id
            routePacket->sourceid = source_id;                                          // Set the source id
        
            int j;                                                                      // Loop counter

            // For each column, Find the min and add it to the array containing the all the mins
            for(j = 0; j<MAX_NODES; j++){
                routePacket->mincost[j] = compute_min_for_column(j, distanceTable);
            }

            toLayer2(*routePacket);                                                     // Send the packet
        }
    }
}

/*
 * Computes the min value of a column in a distance table
 * @param column The column to compute the min for
 * @param distanceTable The pointer to the distance table to find the min column value for
 * @return the minimum value of the specified column in the distance table
 */
int compute_min_for_column(int column, struct distance_table *distanceTable){
    int i;                                                          // Loop counter
    int min = INFINITY;                                             // Initially min is the minimum

    for(i = 0; i < MAX_NODES; i++){                                 // Loop over each row in the column
        int temp = distanceTable->costs[i][column];                     // Get the value of the current column
        min = temp < min ? temp : min;                                  // Determine if the min should be infinity or the temp
    }

    return min;                                                     // return the result
}

int try_set_min_cost(struct RoutePacket *rcvdpkt, struct distance_table *distanceTable){
    // Grab the source_id from the received packet
    int source_id = rcvdpkt->sourceid;
    int needsUpdate = 0;                                    // Indicates if the min cost was updated
    int i;                                                  // Loop counter

    // Loop through all possible nodes
    for(i = 0; i < MAX_NODES; i++)
    {
        int *current = &(distanceTable->costs[i][source_id]);
        int new_min = rcvdpkt->mincost[i] + distanceTable->costs[source_id][source_id];
        
        if (*current > new_min && new_min >= 0){
            *current = new_min;
            needsUpdate++;
        }
    }

    return needsUpdate;
}






/*
 * Compares the current min to a  new one.
 * if the new min is non negative and less than current then updated
 * current to new.
 * @param currentMin Pointer to a value to the distance table that will potentially be updated
 * @param newMin Value to compare current min to.
 * @return 1 if currentMin was updated 0 otherwise
 */
// int try_set_min_cost(int *currentMin, int newMin);{
//     if (*currentMinat > newMin && newMin >= 0){
//         *currentMinat = newMin;
//         return 1;
//     }
//     return 0;
// }
