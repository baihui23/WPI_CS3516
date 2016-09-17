/* Author: Jules Voltaire - javoltaire */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

// Constants
#define P_OPTION "-p"
#define GET_FORMAT "GET %s HTTP/1.1\r\nHOST: %s\r\nConnection: close\r\n\r\n"

// Function prototypes
int tryToConnect(char *host, int portNumber);                               // looks up a host and tries to connect              
void disconnect(int sockDescriptor);                                        // Closes the given descriptor
int isOption(char* arg);                                                    // Indicates wheter the given character array starts with '-'
void showHelp();                                                            // Prints a series messages on how to use this program

/**
 * This main method is the starting point of this program, it takes a number of arguments which will be used as options, address and port number;
 The address  and port number are required arguments.
 */
int main(int argc, char *argv[]){
    char* host;                                     // The host to connect to can be ip or name
    char path[1024];                                     // The path to the file on the server if there is one
    int portNumber;                                 // Port number to be used
    int sockDescriptor;                             // Socket file descriptor
    int useP = 0;                                   // Indicates whether the p option should be use.

    // Checking to make sure that we have enough arguments
    if(argc < 3){
        printf("ERROR: You need give an address url/ip and and a port number.\n");
        showHelp();
        return 1;
    }
    else{
        int i;                                                                              // Loop counter
        char *fullAddress = argv[argc - 2];                                                 // The full address entered by the user.
        char *pathPtr;
        struct timeval start, end;

        // Making sure that this argument is not an option
        if(isOption(fullAddress)){
            printf("ERROR: Invalid Address.\n");
            return 1;
        }
        
        pathPtr = strstr(fullAddress, "/");                                                    // Locate a '/' and everything after it.
        if(pathPtr == NULL){                                                                   // If there were no '/' and something after it
            strcpy(path, "/");
            host = fullAddress;                                                                 // host is the whole string
        }
        else{
            strcpy(path, pathPtr);
            host = strtok(fullAddress, "/");
        }        

        // Getting the port number argument;
        portNumber = atoi(argv[argc - 1]);     // The port number should be the last argument
        printf("%d\n", portNumber);

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

        if((sockDescriptor = tryToConnect(host, portNumber)) < 0){
            printf("ERROR: Connection failed");
            return 1;
        }

        // Create Get Message
        char getMessage[1024];
        sprintf(getMessage, GET_FORMAT, path, host);

        // Loging the current datetime before transmission
        gettimeofday(&start, NULL);

        //Sending the message to the server
        if(write(sockDescriptor, getMessage, strlen(getMessage)) < 0){
            perror("Error: Could not write to socket");
            return 1;
        }

        printf("Success: Sucessfully send message to server.\n");            // <================================================================================ DELETE =========

        int readResult = 0;
        char resultBuffer[6000];
        memset(resultBuffer, '0', sizeof(resultBuffer));

        while((readResult = read(sockDescriptor, resultBuffer, sizeof(resultBuffer))) > 0){
            resultBuffer[readResult] = 0;
            printf("%s", resultBuffer);
        }

        if(readResult < 0)
        {
            printf("ERROR: %s", strerror(errno));
        }

        printf("\n");

        // Loging the current datetime after receiving data
        gettimeofday(&end, NULL);

        if(useP){
            double rtt = (end.tv_sec- start.tv_sec)* 1000 + (end.tv_usec - start.tv_usec) / 1000;
            printf("RTT: %.2f Ms\n", rtt);
        }

        printf("Success: Sucessfully received message to server.\n");            // <================================================================================ DELETE =========

        // Closing socket
        close(sockDescriptor);
        printf("Success: Socket Closed.\n");            // <================================================================================ DELETE =========
        
        // printf("%s\n", serverResponse);
    }
    return 0;
}

/**
 * Looks up the given host and tries to connect
 * @param host The host to look up
 * @param portNumber The port number to use
 * @return socket descripto on success -1 otherwise
 */
int tryToConnect(char *host, int portNumber){
    int sockDescriptor; 
    struct addrinfo hints;                                                                  // Criteria for the address look up
    struct addrinfo *result;                                                                // Will hold address infos that were returned
    struct addrinfo *loopCounter;                                                           // Will point to the result (above) so we can loop over all of them
    int lookUpResult;                                                                       // Will hold the result of the look up, success or failure

    // Converting the port number to a string
    char servname[5];
    sprintf(servname, "%d", portNumber);

    memset(&hints, 0, sizeof hints);                                                        // Zeroing the hints structure

    // Setting value to some of the  members of the hints structure
    hints.ai_family = AF_UNSPEC;                                                            // Look for both ipv4 and ipv6
    hints.ai_socktype = SOCK_STREAM;

    lookUpResult = getaddrinfo(host, servname, &hints, &result);                         // Make the call to look up the host

    // Check if the result was successful or not.
    if(lookUpResult != 0){
        printf("ERROR: Something went wrong while looking up the host. %s\n", gai_strerror(lookUpResult));
        return -1;
    }

    // Loop through all the addrinfo structs until we find one to connect to.
    for(loopCounter = result; loopCounter != NULL; loopCounter = loopCounter->ai_next){
        sockDescriptor = socket(loopCounter->ai_family, loopCounter->ai_socktype, loopCounter->ai_protocol);                  // Create a new socket struct
        // Check to make sure the socket was successfully created
        if (sockDescriptor < 0) {
            // Failed!!
            perror("ERROR: ");
            continue;                                                                               // Try again
        }

        printf("Success: Socket Created.\n");                           // <================================================================================ DELETE =========

        // Try to connect
        if (connect(sockDescriptor, loopCounter->ai_addr, loopCounter->ai_addrlen) < 0) {
            // Failed!!!
            perror("ERROR: ");
            close(sockDescriptor);                                                                  // Close descriptor
            continue;                                                                               // Try again
        }

        printf("Success: Connection sucessfully Created.\n");            // <================================================================================ DELETE =========

        break;                                                                                   // Connection successful
    }

    // If we get here we didn't connect to a single one of the results
    if (loopCounter == NULL) {
        printf("ERROR: Failed to connect to server.\n");
        return -1;
    }

    freeaddrinfo(result);
    return sockDescriptor;
} 

/**
 * Closes descriptor
 * @param sockDescriptor the desctiptor to close
 */
void disconnect(int sockDescriptor){
    close(sockDescriptor);
}

/**
 * This method checks if the argument passed starts with '-' which would mean that the string is an option.
 * @param arg The string to be checked
 * @return 1 if agr starts with '-' 0 otherwise
 */
int isOption(char* arg){
    return arg[0] == '-';
}

/**
 * Prints messages on how to use this program to the screen.
 */
void showHelp(){
    printf("ERROR: Invalid number of arguments");
    printf("USAGE: ./http_client [-options] server_address port_number\n");
    printf("Example: ./http_client www.cnn.com 80\n");
    printf("or\n");
    printf("Example: ./ http_client –p www.cnn.com 80\n");
    printf("port_number should be between 0 and 65536, inclusive\n");
    printf("Use 80 for http.\n");
    printf("If port number is invalid, 0 is used\n");
    printf("Each option should be separated by a space character.\n");
    printf("Invalid options are ignored.\n");
    printf("Avalaible options:\n");
    printf("-p: Prints the RTT for accessing the address before the server's response.\n");
}
