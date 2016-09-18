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
#define P_OPTION "-p"                                                               // The -p option that the user can specify to print rtt time to the screen
#define GET_FORMAT "GET %s HTTP/1.1\r\nHOST: %s\r\nConnection: close\r\n\r\n"       // The format of the get message to be sent to the server, host and path needs to be specified

// Function prototypes
int tryToConnect(char *host, int portNumber);                                       // looks up a host and tries to connect
int isOption(char* arg);                                                            // Indicates wheter the given string starts with '-', which means the string is an option e.g -p
void showHelp();                                                                    // Prints a series messages on how to use this program

/**
 * This main method is the starting point of this program, it takes a number of arguments which will be used as options, address and port number;
 The address  and port number are required arguments.
 */
int main(int argc, char *argv[]){
    int portNumber;                                                                 // Port number to be used
    int sockDescriptor;                                                             // Socket file descriptor
    int useP = 0;                                                                   // Indicates whether the p option should be use.

    if(argc < 3){                                                                   // Checking to make sure that we have enough arguments
        printf("ERROR: You need give an address url/ip and and a port number.\n");
        showHelp();
        return 1;
    }
    else{
        char *fullAddress = argv[argc - 2];                                                 // The full address entered by the user.
        char* host;                                                                         // Will hold the host to connect to, can be ip or name
        char path[1024];                                                                    // Will hold the path to the file need on the server
        char *pathPtr;                                                                      // Will hold a string with the path and everything after it.
        int i;                                                                              // Loop counter
        struct timeval start, end;                                                          // Struct that will hold timestamps for the rtt time calculations

        if(isOption(fullAddress)){                                                          // Making sure that this argument is not an option
            printf("ERROR: Invalid Address.\n");
            return 1;
        }

        pathPtr = strstr(fullAddress, "/");                                                 // Locate a '/' and everything after it.
        if(pathPtr == NULL){                                                                // If there were no '/' and something after it
            strcpy(path, "/");                                                                  // make the path '/'
            host = fullAddress;                                                                 // host is the whole string
        }
        else{                                                                               // otherwise
            strcpy(path, pathPtr);                                                              // copy the path in the path variable
            host = strtok(fullAddress, "/");                                                    // And the host is all the things before it.
        }    

        // Getting the port number argument;
        portNumber = atoi(argv[argc - 1]);                                                  // The port number should be the last argument

        if (portNumber > 65536 || portNumber <= 0) {                                        //Checking to make sure the port number is okay to use.
            printf("ERROR: Invalid Port Number!\n");
            return 1;
        }

        // Checking if we have more than 3 arguments, which means there should be options to parse
        if(argc > 3){
            for(i = 1; i < argc - 2; i++){                                                      // Looping through all the arguments starting with the second one.
                if(!strcmp(P_OPTION, argv[i])){                                                     // Check if the argument is the p option.
                    useP = 1;                                                                           // Indicate the p option should be used.
                }
            }
        }

        printf("Connecting...\n");

        if((sockDescriptor = tryToConnect(host, portNumber)) < 0){                          // Try to connect using the host and port number.
            printf("ERROR: Connection failed\n");
            return 1;
        }

        printf("SUCCESS: Connected to server.\n");

        char getMessage[1024];                                                              // Will hold the message to be sent to the server
        sprintf(getMessage, GET_FORMAT, path, host);                                        // Format the message.

        gettimeofday(&start, NULL);                                                         // Log the current datetime before transmission

        if(write(sockDescriptor, getMessage, strlen(getMessage)) < 0){                      // Try to send the message to the server
            perror("SOCK_WRITTING_ERROR:");
            close(sockDescriptor);
            return 1;
        }

        printf("SUCCESS: Message sent to server.\n");

        int readResult = 0;                                                                 // Will hold the result of the read
        char resultBuffer[6000];                                                            // Buffer to hold the part of the message sent back from the server
        memset(resultBuffer, '0', sizeof(resultBuffer));                                    // 0 the buffer

        while((readResult = read(sockDescriptor, resultBuffer, sizeof(resultBuffer))) > 0){ // Loop and read all the message
            resultBuffer[readResult] = 0;                                                       // make the last character 0
            printf("%s", resultBuffer);                                                         // Print the buffer to the screen
        }

        if(readResult < 0)                                                                  // Check if the read result is okay
        {
            printf("ERROR: %s", strerror(errno));
            close(sockDescriptor);
            return 1;
        }

        printf("\n");
        
        gettimeofday(&end, NULL);                                                           // Log the current datetime after receiving data

        if(useP){                                                                           // Check if the rtt should be printed out
            double rtt = (end.tv_sec- start.tv_sec)* 1000 + (end.tv_usec - start.tv_usec) / 1000;// Calculate the rtt
            printf("RTT: %.2f Ms\n", rtt);                                                      // Print the rtt
        }

        close(sockDescriptor);                                                              // Close the socket
        printf("SUCCESS: Socket Closed.\n");
    }
    return 0;
}

/**
 * Looks up the given host and tries to connect
 * @param host The host to look up
 * @param portNumber The port number to use
 * @return socket descriptor on success -1 otherwise
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

        // Try to connect
        if (connect(sockDescriptor, loopCounter->ai_addr, loopCounter->ai_addrlen) < 0) {
            // Failed!!!
            perror("ERROR: ");
            close(sockDescriptor);                                                                  // Close descriptor
            continue;                                                                               // Try again
        }
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
    printf("port_number should be between 1 and 65536, inclusive\n");
    printf("Use 80 for http.\n");
    printf("Each option should be separated by a space character.\n");
    printf("Invalid options are ignored.\n");
    printf("Avalaible options:\n");
    printf("-p: Prints the RTT for accessing the address before the server's response.\n");
}
