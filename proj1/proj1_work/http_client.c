#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Constants
#define P_OPTION "-p"

// Function prototypes
void showHelp();                                // Prints a series messages on how to use this program

/**
 * This main method is the starting point of this program, it takes a number of arguments which will be used as options, url and port number;
 The address  and port number are required arguments.
 */
int main(int argc, char *argv[]){
    int error_result = 0;                       // int to return as a result for this function;

    // Checking to make sure that we have enough arguments
    if(argc < 4){
        printf("ERROR: You need give a url and and a port number.\n");
        showHelp();
        error_result = 1;
    }
    else{

    }
    return error_result;
}

/**
 * Prints messages on how to use this program to the screen.
 */
void showHelp(){
    printf("USAGE: ./http_client [-options] server_url port_number\n");
    printf("Example: ./http_client www.cnn.com 80\n");
    printf("or\n");
    printf("Example: ./ http_client –p www.cnn.com 80\n");
    printf("Avalaible options:\n");
    printf("-p: Prints the RTT for accessing the URL before the server's response.\n");
}