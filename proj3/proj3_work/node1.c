#include "node_helper.h"

#define NODE_ID 1

// Function prototypes
void printdt1( int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr );

// extern int TraceLevel;
// extern float clocktime;

struct distance_table dt1;
struct NeighborCosts   *neighbor1;

/* students to write the following two routines, and maybe some others */

void rtinit1() {
    // Print the trace (trace level is takken into account, no need to check again here.)
    print_trace("rtinit1()");

    // Initializing all nodes' cost in node1's distance_table to infinity
    init_to_infinity(&dt1);
    
    // Grab the cost for neighbors
    neighbor1 = getNeighborCosts(NODE_ID);

    // Set the direct nodes
    set_direct_adjacent_costs(neighbor1, &dt1);

    // Send to all the neighbors
    send_to_neighbors(NODE_ID, neighbor1->NodesInNetwork, &dt1);

    // Print
    printdt1(NODE_ID, neighbor1, &dt1);
}


void rtupdate1( struct RoutePacket *rcvdpkt ) {
    // Print the trace (trace level is takken into account, no need to check again here.)
    print_trace("rtupdate1()");

    int needsUpdate = try_set_min_cost(rcvdpkt, &dt1);

    if(needsUpdate){
        // Send to all the neighbors
        send_to_neighbors(NODE_ID, neighbor1->NodesInNetwork, &dt1);
    }

    // Print
    printdt1(NODE_ID, neighbor1, &dt1);
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
void printdt1( int MyNodeNumber, struct NeighborCosts *neighbor, struct distance_table *dtptr ) {
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
}    // End of printdt1

