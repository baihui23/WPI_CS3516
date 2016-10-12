#include <stdio.h>
#include <stdlib.h>
#include "project3.h"

#define NODE_ID 0

extern int TraceLevel;
extern float clocktime;

struct distance_table {
  int costs[MAX_NODES][MAX_NODES];
};
struct distance_table dt0;
struct NeighborCosts   *neighbor0;

// Function prototypes
void print_trace(char *methodCalled);
void init_to_infinity(struct distance_table *distanceTable);
void set_direct_adjacent_costs( struct NeighborCosts *neighborCosts, struct distance_table *distanceTable);
void send_to_neighbors(int source_id, int numNodesInNetwork, struct distance_table *distanceTable);
int compute_min_for_column(int column, struct distance_table *distanceTable);
void printdt0( int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr );



/* students to write the following two routines, and maybe some others */

/*
 * This routine will be called once at the beginning of the emulation. rtinit0() 
 * has no arguments. It should initialize the distance table in node0 to reflect 
 * the direct costs to is neighbors by using GetNeighborCosts().
 */
void rtinit0() {
    // Print the trace (trace level is takken into account, no need to check again here.)
    print_trace("rtinit0()");

    // Initializing all nodes' cost in node0's distance_table to infinity
    init_to_infinity(&dt0);
    
    // Grab the cost for neighbors
    neighbor0 = getNeighborCosts(0);

    // Set the direct nodes
    set_direct_adjacent_costs(neighbor0, &dt0);

    // Send to all the neighbors
    send_to_neighbors(NODE_ID, neighbor0->NodesInNetwork, &dt0);

    // Print
    printdt0(NODE_ID, neighbor0, &dt0);

}

void rtupdate0(struct RoutePacket *rcvdpkt) {
    // Print the trace (trace level is takken into account, no need to check again here.)
    print_trace("rtupdate0()");
}

// ----------------------- General Helper Methods ----------------------------

/*
 * Prints the method called being passed as string parameter
 *@param methodCalled The method that is called and that needs to be traced
 */
void print_trace(char *methodCalled){
    // If there is a high enough level for tracing pring the clock time when this is called
    if (TraceLevel >= 0)
    {
        printf("At time t=%f, %s called.\n", clocktime, methodCalled);
    }
}

/*
 * Sets all the cost in the given distance table a value of infinity
 * @param distanceTable the distance table who's nodes need to be set to infinity
 */
void init_to_infinity(struct distance_table *distanceTable){
    int row;                                                                    // row loop counter
    int column;                                                                 // column loop counter

    // Loop and assign infinity
    for(row = 0; row < MAX_NODES; row++)
    {
        for(column = 0; column < MAX_NODES; column++)
        {
            distanceTable->costs[row][column] = INFINITY;
        }
    }   
}

/*
 * Sets the cost of the nodes directly connected
 * @param distanceTable table that contains distances that will be updated
 * @param neighborCosts struct that contains costs of pahts
 */
void set_direct_adjacent_costs(struct NeighborCosts *ncs, struct distance_table *distanceTable){
    int i;                                                                          // Loop counter

    // Loop over all the nodes in the network
    for(i = 0; i < ncs->NodesInNetwork; i++)
    {
        distanceTable->costs[i][i] = ncs->NodeCosts[i];
    }
}

/*
 * This method creates a packet for each node in the network then computes the min cost of for each colum in
 * the distance table and one by one sends them to neighbors
 */
void send_to_neighbors(int source_id, int numNodesInNetwork, struct distance_table *distanceTable){
    int i;
    for(i = 0; i < numNodesInNetwork; i++)
    {
        if(i != source_id){
            struct RoutePacket *routePacket = (struct RoutePacket *)malloc(sizeof(struct RoutePacket));
            routePacket->destid = i;
            routePacket->sourceid = source_id;
        
            int j;
            for(j = 0; j<MAX_NODES; j++)
            {
                routePacket->mincost[j] = compute_min_for_column(j, distanceTable);
            }

            toLayer2(*routePacket);
        }
    }
}

/*
 * Computes the min value of a column in a distance table
 * @param column the column number to compute for the table
 * @param dt The table to compute value for
 * @return the minimum value of the column in the table
 */
int compute_min_for_column(int column, struct distance_table *dt){
    int i;
    int min = INFINITY;

    for(i = 0; i < MAX_NODES; i++)
    {
        int temp = dt->costs[i][column];
        min = temp < min ? temp : min;
    }

    return min;
}

// ------------------------- End of helper section ---------------------------------

/////////////////////////////////////////////////////////////////////
//  printdt
//  This routine is being supplied to you.  It is the same code in
//  each node and is tailored based on the input arguments.
//  Required arguments:
//  MyNodeNumber:  This routine assumes that you know your node
//                 number and supply it when making this call.
//  struct NeighborCosts *neighbor:  A pointer to the structure 
//                 that's supplied via a call to getNeighborCosts().
//                 It tells this print routine the configuration
//                 of nodes surrounding the node we're working on.
//  struct distance_table *dtptr: This is the running record of the
//                 current costs as seen by this node.  It is 
//                 constantly updated as the node gets new
//                 messages from other nodes.
/////////////////////////////////////////////////////////////////////
void printdt0( int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr ) {
    int       i, j;
    int       TotalNodes = neighbor->NodesInNetwork;     // Total nodes in network
    int       NumberOfNeighbors = 0;                     // How many neighbors
    int       Neighbors[MAX_NODES];                      // Who are the neighbors

    // Determine our neighbors 
    for ( i = 0; i < TotalNodes; i++ )  {
        if (( neighbor->NodeCosts[i] != INFINITY ) && i != MyNodeNumber )  {
            Neighbors[NumberOfNeighbors] = i;
            NumberOfNeighbors++;
        }
    }
    // Print the header
    printf("                via     \n");
    printf("   D%d |", MyNodeNumber );
    for ( i = 0; i < NumberOfNeighbors; i++ )
        printf("     %d", Neighbors[i]);
    printf("\n");
    printf("  ----|-------------------------------\n");

    // For each node, print the cost by travelling thru each of our neighbors
    for ( i = 0; i < TotalNodes; i++ )   {
        if ( i != MyNodeNumber )  {
            printf("dest %d|", i );
            for ( j = 0; j < NumberOfNeighbors; j++ )  {
                    printf( "  %4d", dtptr->costs[i][Neighbors[j]] );
            }
            printf("\n");
        }
    }
    printf("\n");
}    // End of printdt0

