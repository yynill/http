#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/server.h"

#define PORT 8080

int start_server()
{
    printf("Starting HTTP server...\n");

    // create and write to a file
    // FILE *file = fopen("output.txt", "w");
    // fprintf(file, "Hello, File!\n");
    // fclose(file);

    // Step 1: Create the socket

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
    {
        perror("❌ soceket failure");
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");

    // Step 2: Bind the socket to an IP address and port
    struct sockaddr_in address;
    int addrlen = sizeof(address);

    address.sin_family = AF_INET;         // Use IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Accept connections from any IP address
    address.sin_port = htons(PORT);       // Port to listen on

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("❌ bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket bound to port %d.\n", PORT);

    // Step 3: Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("❌listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Listening for incoming connections...\n");

    // Step 4: Accept incoming connections
    int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
    if (new_socket < 0)
    {
        perror("❌accept failed");
        exit(EXIT_FAILURE);
    }
    printf("Connection accepted.\n");

    // TODO: handle the request from the client...

    // Close the new socket (communication socket)
    close(new_socket);

    // Close the server socket (after done listening)
    close(server_fd);

    return 0;
}
