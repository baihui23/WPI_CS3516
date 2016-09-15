/* Author: Jules Voltaire - javoltaire */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

// Constants
#define P_OPTION "-p"
#define GET_FORMAT "GET %s HTTP/1.0\nHOST: %s\r\n"

// Function prototypes
int tryToConnect(int sockDescriptor, char *address, int portNumber);        // looks up a host and tries to connect              
void disconnect(int sockDescriptor);                                        // Closes the given descriptor
int isOption(char* arg);                                                    // Indicates wheter the given character array starts with '-'
void showHelp();                                                            // Prints a series messages on how to use this program

/**
 * This main method is the starting point of this program, it takes a number of arguments which will be used as options, address and port number;
 The address  and port number are required arguments.
 */
int main(int argc, char *argv[]){
    char* address;                                  // The address to request
    int portNumber;                                 // Port number to be used
    int sockDescriptor;                              // Socket file descriptor
    int useP = 0;                                   // Indicates whether the p option should be use.

    // Checking to make sure that we have enough arguments
    if(argc < 3){
        printf("ERROR: You need give an address url/ip and and a port number.\n");
        showHelp();
        return 1;
    }
    else{
        int i;                                  // Loop counter
        // Getting the address
        address = argv[argc - 2];                   // The address is the second to last argument;

        // Making sure that this argument is not an option
        if(isOption(address)){
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

        if(tryToConnect(sockDescriptor, address, portNumber) == 0){
            printf("ERROR: Connection failed");
            return 1;
        }
    }
    return 0;
}

/**
 * Looks up the given address and tries to connect
 * @param sockDescriptor
 * @param address The address to look up
 * @param portNumber The port number to use
 * @return 1 on success 0 otherwise
 */
int tryToConnect(int sockDescriptor, char *address, int portNumber){
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

    lookUpResult = getaddrinfo(address, servname, &hints, &result);                         // Make the call to look up the address

    // Check if the result was successful or not.
    if(lookUpResult != 0){
        printf("ERROR: Something went wrong while looking up the address. %s\n", gai_strerror(lookUpResult));
        return 0;
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
        return 0;
    }

    freeaddrinfo(result);                                                                       // No longer needed
    return 1;
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
    printf("USAGE: ./http_client [-options] server_address port_number\n");
    printf("Example: ./http_client www.cnn.com 80\n");
    printf("or\n");
    printf("Example: ./ http_client –p www.cnn.com 80\n");
    printf("If port number is invalid, 0 is used\n");
    printf("Each option should be separated by a space character.\n");
    printf("Invalid options are ignored.\n");
    printf("Avalaible options:\n");
    printf("-p: Prints the RTT for accessing the address before the server's response.\n");
}




























// int execute(int useP, char* address, int portNumber);   // Executes the user request to get the file
// char* findIPForHost(char* hostName);                    // Looks up the ip address for hostName
// int isIPValid(char* ip);                                // Makes sure the argument passed in is a valid ip address

// /**
//  * Executes the user request to get the file
//  * @param useP Value indicating wheter the -p option should be used.
//  * @param address The address to the destination server.
//  * @param portNumber The port number that should be used to connect to the server.
//  */
// int execute(int useP, char* address, int portNumber){
//     // Creating a socket
//     int sockDescriptor = socket(AF_INET, SOCK_STREAM, 0);
//     struct sockaddr_in destinationServer;

//     // Making sure that the socket was actually created
//     if(sockDescriptor < 0){
//         printf("ERROR: Could not create socket. %s\n", strerror(errno));
//         return 1;
//     }

//     printf("Success: Socket Created.\n");                           // <================================================================================ DELETE =========

//     // Parsing the address
//     if(isIPValid(address)){                         // Is the address an ip
//         destinationServer.sin_addr.s_addr = inet_addr(address);
//         destinationServer.sin_family = AF_INET;
//         destinationServer.sin_port = htons(portNumber);

//         //Connect to remote server
//         if (connect(sockDescriptor , (struct sockaddr *)&destinationServer , sizeof(destinationServer)) < 0)
//         {
//             printf("ERROR: Could not connect to %s.\n%s", address, strerror(errno));
//             return 1;
//         }
//         printf("Success: Connection sucessfully Created.\n");            // <================================================================================ DELETE =========

//         char message[1024];
//         sprintf(message, GET_FORMAT, "/", address);
//         if( send(sockDescriptor , message , strlen(message) , 0) < 0)
//         {
//             printf("ERROR: Could not send data to server. %s\n", strerror(errno));
//             return 1;
//         }
//         printf("Success: Sucessfully sent data to server.\n");            // <================================================================================ DELETE =========

//         char server_reply[2048];
//         //Receive a reply from the server
//         if(recv(sockDescriptor, server_reply , 2048, 0) < 0)
//         {
//             printf("recv failed");
//         }
//         printf("Reply received\n");
//         printf("%s\n", server_reply);
//     }
//     else{                                           // Then a host name/ domain is given

//     }


//     return 0;
// }

// /**
//  * Finds the address of the given host name
//  * @param hostName a string representing the host name that needs to get an ip for
//  * @return the ip address for the given host name
//  */
// char *findIPForHost(char* hostName){

//     // Look up the host name;
//     he = gethostbyname(hostName);

//     // Check if something was return
//     if(he == NULL){
        
//     }

// }

// /**
//  * Validates the string to make sure that it is a valid isIPValid
//  * @param ip the supposed ip address to be checked
//  * @return 1 if valid, 0 if not ip is not valid in AF_INET and -1 if 
//  */
// int isIPValid(char* ip){
//     struct sockaddr_in address;
//     int result = inet_pton(AF_INET, ip, &(address.sin_addr));
//     if(result < 0){
//         printf("ERROR: Encountered an error while checking the ip address.\n%s", strerror(errno));
//         exit(1);
//     }
//     return result;
// }