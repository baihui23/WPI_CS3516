/* Author: Jules Voltaire - javoltaire */
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <pthread.h>

// Constants
#define GET "GET"
#define HTTP "HTTP/1.1"
#define ROOT "/"
#define NEWLINE "\r\n"
#define STATUS_OK "200 OK\n"

// Function prototypes
void printHelp();                                          // A method to print usage and such to the user
int tryToBind(char *servname);                             // Tries to bind to the port number
void *processAcceptedSockDesctiptor(void *sockDescriptor); // Processes a request

/**
 * This main method is the starting point of this program, the only argument that should be passed in the port number.
 */
int main(int argc, char *argv[])
{
    // Make sure the we have the right number of arguments
    if (argc != 2)
    {
        printHelp();
        return 1;
    }
    else
    {
        char *servname = argv[1]; // Grabbing the port/protool

        printf("Connecting...\n");

        int listenSockDescriptor = tryToBind(servname); // Try to bind/connect

        printf("Connection Sucessful.\n");

        if (listenSockDescriptor < 0)
        { // Check if the descriptor from the connection is okay
            printf("Failed to Bind\n");
            return 1;
        }

        if (listen(listenSockDescriptor, 10) < 0)
        { // Now listen, queue the connection requests
            printf("Error: Listen Failed. %s", strerror(errno));
            return 1;
        }

        printf("Listening Successful. Waiting for connections...\n");

        int acceptSockDescriptor = 0; // Will hold the descriptor for a connection
        pthread_t processThread;      // The thread that will process the request
        while (1)
        {                                                                                                      // Loop forever
            acceptSockDescriptor = accept(listenSockDescriptor, NULL, NULL);                                   // If we get a connection
            pthread_create(&processThread, NULL, processAcceptedSockDesctiptor, (void *)acceptSockDescriptor); // Create a new thread and process the connection.
            pthread_detach(processThread);                                                                     // Detatch from the calling thread.
        }
        close(listenSockDescriptor); // Close the listening port
    }
    return 0;
}

/**
 * Looks up information about the server and tries to connect
 * @param portNumber The port number to use
 * @return socket descripto on success -1 otherwise
 */
int tryToBind(char *servname)
{
    int sockDescriptor;
    struct addrinfo hints;        // Criteria for the address look up
    struct addrinfo *result;      // Will hold address infos that were returned
    struct addrinfo *loopCounter; // Will point to the result (above) so we can loop over all of them
    int lookUpResult;             // Will hold the result of the look up, success or failure

    memset(&hints, 0, sizeof hints); // Zeroing the hints structure

    // Setting value to some of the  members of the hints structure
    hints.ai_family = AF_UNSPEC; // Look for both ipv4 and ipv6
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    lookUpResult = getaddrinfo(NULL, servname, &hints, &result); // Make the call to look up the host

    if (lookUpResult != 0)
    { // Check if the result was successful or not.
        printf("GETADDRINFO%s\n", gai_strerror(lookUpResult));
        return -1;
    }

    // Loop through all the addrinfo structs until we find one to bind to.
    for (loopCounter = result; loopCounter != NULL; loopCounter = loopCounter->ai_next)
    {
        sockDescriptor = socket(loopCounter->ai_family, loopCounter->ai_socktype, loopCounter->ai_protocol); // Create a new socket struct
        // Check to make sure the socket was successfully created
        if (sockDescriptor < 0)
        {
            // Failed!!
            perror("ERROR\n");
            continue; // Try again
        }

        int yes = 1;
        if (setsockopt(sockDescriptor, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            perror("ERROR");
            return -1;
        }

        // Try to Bind
        if (bind(sockDescriptor, loopCounter->ai_addr, loopCounter->ai_addrlen) < 0)
        {
            // Failed!!!
            perror("BINDING ERROR:");
            close(sockDescriptor); // Close descriptor
            continue;              // Try again
        }

        break; // Binding successful
    }

    // If we get here we didn't connect to a single one of the results
    if (loopCounter == NULL)
    {
        printf("ERROR: Failed to Bind.\n");
        return -1;
    }

    freeaddrinfo(result);
    return sockDescriptor;
}

/**
 * Receives a message on a socket and processes the request
 * @param sockDescriptor The socket descriptor
 * @return NULL on completion and on error to exit the calling thread
 */
void *processAcceptedSockDesctiptor(void *sockDescriptor)
{
    int acceptSockDescriptor = (int)sockDescriptor;     // Sock descriptor to be used
    if (acceptSockDescriptor < 0)
    {
        perror("ERROR: Failed to accept connection");
        return NULL;
    }

    // Reading the entire message
    int readResult = 0;
    char resultBuffer[1024];
    memset(resultBuffer, '0', sizeof(resultBuffer));

    printf("Receiving Message.");

    while ((readResult = read(acceptSockDescriptor, resultBuffer, sizeof(resultBuffer))) > 0)       // Read the message
    {
        resultBuffer[readResult] = 0;
        printf("..");
        if (strstr(resultBuffer, NEWLINE))
        {
            break;
        }
    }

    if (readResult < 0)
    {
        printf("ERROR: %s", strerror(errno));
        return NULL;
    }

    printf("\n");

    //Parsing the message that was sent to the server

    char method[10], path[1024], http[10];

    sscanf(resultBuffer, "%s /%s %s", method, path, http);

    printf("%s %s %s", method, path, http);

    // Making sure that the request method is a get
    if (strcmp(method, GET) != 0)
    {
        //Sending the message to the server
        char *errorMessage = "405 Method Not Allowed\n";
        if (write(acceptSockDescriptor, errorMessage, strlen(errorMessage)) < 0)
        {
            perror("Error: Could not write to socket");
            return NULL;
        }
    }
    else
    {
        //Try to get the file
        FILE *file;
        if (path != NULL || strcmp(path, ROOT) == 0 || strcmp(path, "index.html") == 0)
        {
            file = fopen("TMDG.html", "r");
        }
        else
        {
            file = fopen(path, "r");
        }

        if (file == NULL)
        {
            // If the file is not found send an error message back to the host.
            char *errorMessage = "404 Not Found\n";
            if (write(acceptSockDescriptor, errorMessage, strlen(errorMessage)) < 0)
            {
                perror("Error: Could not write to socket");
                return NULL;
            }
        }
        else
        {
            // Formatting the status message
            char statusMessage[strlen(HTTP) + strlen(STATUS_OK) + 1];
            sprintf(statusMessage, "%s %s\n", HTTP, STATUS_OK);

            // Try to send the status
            if (write(acceptSockDescriptor, statusMessage, strlen(statusMessage)) < 0)
            {
                perror("Error: Could not write to socket");
                return NULL;
            }

            // Read the whole file
            fseek(file, 0, SEEK_END);
            long fileSize = ftell(file);
            fseek(file, 0, SEEK_SET);

            char *fileContent = malloc(fileSize + 1);
            fread(fileContent, fileSize, 1, file);
            fclose(file);

            fileContent[fileSize] = 0;

            if (write(acceptSockDescriptor, fileContent, fileSize) < 0)                     // Send the content back to the host.
            {
                perror("Error: Could not write to socket");
                return NULL;
            }
            free(fileContent);
        }
    }
    close(acceptSockDescriptor);
    return NULL;
}

/**
 * Prints out the Usage and other useful info to the user.
 */
void printHelp()
{
    printf("ERROR: Invalid number of arguments");
    printf("USAGE: ./http_server port_number\n");
    printf("Example: ./http_server 5008\n");
    printf("port_number should be between 1 and 65536, inclusive.\n");
    printf("The higher the better.\n");
}
