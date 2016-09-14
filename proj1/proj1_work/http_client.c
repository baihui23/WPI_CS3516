/* Author: Jules Voltaire - javoltaire */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

// Constants
#define P_OPTION "-p"

// Function prototypes
int execute(int useP, char* url, int portNumber);       // Executes the user request to get the file
int isOption(char* arg);                                // Indicates wheter the given character array starts with '-'
void showHelp();                                        // Prints a series messages on how to use this program

/**
 * This main method is the starting point of this program, it takes a number of arguments which will be used as options, url and port number;
 The address  and port number are required arguments.
 */
int main(int argc, char *argv[]){
    char* url;                                  // The url to request
    int portNumber;                            // Port number to be used

    int useP = 0;                               // Indicates whether the p option should be use.

    // Checking to make sure that we have enough arguments
    if(argc < 3){
        printf("ERROR: You need give a url/ip and and a port number.\n");
        showHelp();
        return 1;
    }
    else{
        int i;                                  // Loop counter
        // Getting the url
        url = argv[argc - 2];                   // The url is the second to last argument;

        // Making sure that this argument is not an option
        if(isOption(url)){
            printf("ERROR: Invalid Address.\n");
            return 1;
        }

        // Getting the port number argument;
        portNumber = atoi(argv[argc - 1]);     // The port number should be the last argument

        //Checking to make sure the port number is okay to use.
        if (portNumber > 65536 || portNumber < 0) {
            printf("ERROR: Invalid Port Number!\n");
            return 1;
        }

        // Checking if we have more than 3 arguments, which means there should be options to parse
        if(argc > 3){
            for(i = 1; i < argc - 2; i++){              // Looping through all the arguments starting with the second one.
                if(!strcmp(P_OPTION, argv[i])){
                    useP = 1;
                }
            }
        }

        return execute(useP, url, portNumber);
    }
    return 0;
}

/**
 * Executes the user request to get the file
 */
int execute(int useP, char* url, int portNumber){
    // Creating a socket
    return 0;
}

/**
 * This method checks if the argument passed starts with '-' which would mean that the string is an option.
 */
int isOption(char* arg){
    return arg[0] == '-';
}

/**
 * Prints messages on how to use this program to the screen.
 */
void showHelp(){
    printf("USAGE: ./http_client [-options] server_url port_number\n");
    printf("Example: ./http_client www.cnn.com 80\n");
    printf("or\n");
    printf("Example: ./ http_client –p www.cnn.com 80\n");
    printf("If port number is invalid, 0 is used\n");
    printf("Each option should be separated by a space character.\n");
    printf("Invalid options are ignored.\n");
    printf("Avalaible options:\n");
    printf("-p: Prints the RTT for accessing the URL before the server's response.\n");
}