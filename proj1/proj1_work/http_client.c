/* Author: Jules Voltaire - javoltaire */
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
    int errorResult = 0;                       // Int to return as a result for this function;

    char* url;                                  // The url to request
    int portNumber;                            // Port number to be used

    // Checking to make sure that we have enough arguments
    if(argc < 3){
        printf("ERROR: You need give a url/ip and and a port number.\n");
        showHelp();
        errorResult = 1;
    }
    else{
        int i;                                  // Loop counter
        // Getting the url
        url = argv[argc - 2];                   // The url is the second to last argument;

        // Getting the port number argument;
        portNumber = atoi(argv[argc - 1]);     // The port number should be the last argument

        //Checking to make sure the port number is okay to use.
        if (portNumber > 65536 || portNumber < 0) {
            printf("ERROR: Invalid Port Number!\n");
            exit(1);
        }

        // Checking if we have more than 3 arguments, which means there should be options to parse
        if(argc > 3){
            for(i = 1; i < argc - 2; i++){              // Looping through all the arguments starting with the second one.

            }
        }

        printf("Address:%s on port %d\n", url, portNumber);


    }
    return errorResult;
}

/**
 * Prints messages on how to use this program to the screen.
 */
void showHelp(){
    printf("USAGE: ./http_client [-options] server_url port_number\n");
    printf("Example: ./http_client www.cnn.com 80\n");
    printf("or\n");
    printf("Example: ./ http_client –p www.cnn.com 80\n");
    printf("Each option should be separated by a space character.\n");
    printf("Avalaible options:\n");
    printf("-p: Prints the RTT for accessing the URL before the server's response.\n");
}