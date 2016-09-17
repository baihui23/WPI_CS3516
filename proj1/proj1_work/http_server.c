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
#define GET "GET"
#define HTTP "HTTP/1.1"
#define ROOT "/"
#define NEWLINE "\r\n"
#define STATUS_OK "200 OK\n"

#define GET_FORMAT "GET %s HTTP/1.1\r\nHOST: %s\r\nConnection: close\r\n\r\n"

// Function prototypes
void printHelp();                                                       // A method to print usage and such to the user
int tryToBind(char *servname);

/**
 * This main method is the starting point of this program, the only argument that should be passed in the port number.
 */
int main(int argc, char *argv[]){
    // Make sure the we have the right number of arguments
    if(argc != 2){
        printHelp();
        return 1;
    }
    else{
        // Grabbing the port/protool
        char *servname = argv[1];
        // Try to bind/connect
        int listenSockDescriptor = tryToBind(servname);                                                     // Listening File socket descriptor

        // Now listen, queue the requests
        listen(listenSockDescriptor, 10);

        int acceptSockDescriptor = 0;                                                       // Accepting file socket descriptor
        while(1){
            acceptSockDescriptor = accept(listenSockDescriptor, NULL, NULL);

            // Reading the entire message
            int readResult = 0;
            char resultBuffer[1024];
            memset(resultBuffer, '0', sizeof(resultBuffer));
            
            printf("Receiving Message.");

            while((readResult = read(acceptSockDescriptor, resultBuffer, sizeof(resultBuffer))) > 0){
                resultBuffer[readResult] = 0;
                printf("..");
                if(strstr(resultBuffer, NEWLINE)){break;}
            }

            if(readResult < 0)
            {
                printf("ERROR: %s", strerror(errno));
            }
            
            printf("\n");

            //Parsing the message that was sent to the server

            char method[10], path[1024], http[10];

            sscanf(resultBuffer, "%s /%s %s", method, path, http);
            
            printf("%s %s %s", method, path, http);

            // Making sure that the request method is a get
            if(strcmp(method, GET) != 0){                              
                //Sending the message to the server
                char *errorMessage = "Invalid request method, Use GET";
                if(write(acceptSockDescriptor, errorMessage, strlen(errorMessage)) < 0){
                    perror("Error: Could not write to socket");
                    return 1;
                }
            }
            else{
                //Getting the file
                FILE *file;
                if(path != NULL || strcmp(path, ROOT) == 0){
                    file = fopen("TMDG.html", "r");
                }
                else{
                    file = fopen(path, "r");
                }

                if(file == NULL){
                    char *errorMessage = "404 Not Found\n";
                    if(write(acceptSockDescriptor, errorMessage, strlen(errorMessage)) < 0){
                        perror("Error: Could not write to socket");
                        return 1;
                    }
                }
                else{
                    char * line = NULL;
                    size_t len = 0;
                    ssize_t read;

                    if(write(acceptSockDescriptor, STATUS_OK, strlen(STATUS_OK)) < 0){
                        perror("Error: Could not write to socket");
                        return 1;
                    }

                    while ((read = getline(&line, &len, file)) != -1) {
                        if(write(acceptSockDescriptor, line, len) < 0){
                            perror("Error: Could not write to socket");
                            return 1;
                        }
                        printf("%s", line);
                    }
                    fclose(file);
                    if (line){
                        free(line);
                    }
                }
            }
            close(acceptSockDescriptor);
        }

    }
    return 0;
}

// /**
//  * Looks up information about the server and tries to connect
//  * @param portNumber The port number to use
//  * @return socket descripto on success -1 otherwise
//  */
int tryToBind(char *servname){
    int sockDescriptor; 
    struct addrinfo hints;                                                                  // Criteria for the address look up
    struct addrinfo *result;                                                                // Will hold address infos that were returned
    struct addrinfo *loopCounter;                                                           // Will point to the result (above) so we can loop over all of them
    int lookUpResult;                                                                       // Will hold the result of the look up, success or failure

    memset(&hints, 0, sizeof hints);                                                        // Zeroing the hints structure

    // Setting value to some of the  members of the hints structure
    hints.ai_family = AF_UNSPEC;                                                            // Look for both ipv4 and ipv6
    hints.ai_socktype = SOCK_STREAM;

    lookUpResult = getaddrinfo(NULL, servname, &hints, &result);                         // Make the call to look up the host

    // Check if the result was successful or not.
    if(lookUpResult != 0){
        printf("ERROR: %s\n", gai_strerror(lookUpResult));
        return -1;
    }

    // Loop through all the addrinfo structs until we find one to bind to.
    for(loopCounter = result; loopCounter != NULL; loopCounter = loopCounter->ai_next){
        sockDescriptor = socket(loopCounter->ai_family, loopCounter->ai_socktype, loopCounter->ai_protocol);                  // Create a new socket struct
        // Check to make sure the socket was successfully created
        if (sockDescriptor < 0) {
            // Failed!!
            perror("ERROR: \n");
            continue;                                                                               // Try again
        }

        printf("Success: Socket Created.\n");                           // <================================================================================ DELETE =========

        // Try to Bind
        if (bind(sockDescriptor, loopCounter->ai_addr, loopCounter->ai_addrlen) < 0) {
            // Failed!!!
            perror("ERROR: ");
            close(sockDescriptor);                                                                  // Close descriptor
            continue;                                                                               // Try again
        }

        printf("Success: Binding sucessfull.\n");            // <================================================================================ DELETE =========

        break;                                                                                   // Binding successful
    }

    // If we get here we didn't connect to a single one of the results
    if (loopCounter == NULL) {
        printf("ERROR: Failed to Bind.\n");
        return -1;
    }

    freeaddrinfo(result);
    return sockDescriptor;
} 

void printHelp(){
    printf("ERROR: Invalid number of arguments");
    printf("USAGE: ./http_server port_number\n");
    printf("Example: ./http_server 5008\n");
    printf("port_number should be between 0 and 65536, inclusive.\n");
    printf("The higher the better.\n");
}
