#include "node_helper.h"

#define NODE_ID 0

// extern int TraceLevel;
// extern float clocktime;

struct distance_table dt0;
struct NeighborCosts   *neighbor0;

// Function prototypes
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
    neighbor0 = getNeighborCosts(NODE_ID);

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

