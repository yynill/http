#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/shared.h"

int server_fd; // Make this global so the signal handler can access it

// server won’t leave open sockets behind when Ctrl+C out
void handle_sigint(int sig)
{
    printf("\nCaught SIGINT, shutting down server.\n");
    close(server_fd);
    exit(0);
}

int start_server()
{
    printf("Starting HTTP server...\n");

    // create and write to a file
    // FILE *file = fopen("output.txt", "w");
    // fprintf(file, "Hello, File!\n");
    // fclose(file);

    // Step 1: Create the socket

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
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

    // Persistent server loop
    while (1)
    {

        // Step 4: Accept incoming connections
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            perror("❌accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted.\n");

        // Buffer to store the request
        char buffer[4096] = {0}; // 4KB buffer

        // Read the incoming request
        int valread = read(new_socket, buffer, sizeof(buffer) - 1);
        if (valread < 0)
        {
            perror("❌ read failed");
            close(new_socket);
            exit(EXIT_FAILURE);
        }
        printf("Received request:\n%s\n", buffer);

        // Define a basic HTTP response
        const char *http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/plain\r\n"
            "Content-Length: 13\r\n"
            "\r\n"
            "Hello, World!";

        // Send the response
        write(new_socket, http_response, strlen(http_response));
        printf("Response sent.\n");

        // Close the new socket
        close(new_socket);
    }

    // Close the server socket (after done listening)
    close(server_fd);

    return 0;
}

int main()
{
    // Register signal handler here
    signal(SIGINT, handle_sigint);

    if (start_server() < 0)
    {
        fprintf(stderr, "Failed to start server\n");
        return 1;
    }
    return 0;
}
