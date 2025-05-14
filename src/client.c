#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>  // sockaddr_in, inet_addr
#include <sys/socket.h> // socket, connect
#include "../include/shared.h"
#include "../include/http.h"

int run_client()
{
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    char buffer[1024] = {};

    // Step 1: Create the socket
    if (sock < 0)
    {
        perror("❌ Socket creation error");
        return -1;
    }

    // Step 2: Define the server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0)
    {
        perror("❌ Invalid address / Address not supported");
        return -1;
    }

    // Step 3: Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Connection Failed");
        return -1;
    }

    printf("Connected to server.\n");

    // Step 4: Send a message to the server
    HttpRequest httpreq = {
        .method = "POST",
        .path = "/",
        .host = "localhost",
        .version = "HTTP/1.1",
        .body = "this is a test body"};

    char *req = build_http_request(&httpreq);
    send(sock, req, strlen(req), 0);

    printf("\nRequest: \n%s\n", req);

    // Step 5: Read server response (if any)
    int valread = read(sock, buffer, 1024);
    // Null-terminate the string
    buffer[valread] = '\0';
    printf("\nResponse:\n%s\n", buffer);

    // Step 6: Close the connection
    close(sock);

    return 0;
}

int main()
{
    if (run_client() < 0)
    {
        fprintf(stderr, "Failed to run client\n");
        return 1;
    }
    return 0;
}
