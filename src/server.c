#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/shared.h"
#include "../include/http.h"

int server_fd; // Make this global so the signal handler can access it

// server won’t leave open sockets behind when Ctrl+C out
void handle_sigint()
{
    printf("\nCaught SIGINT, shutting down server.\n");
    close(server_fd);
    exit(0);
}

int start_server()
{
    printf("Starting HTTP server...\n");

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
        char buffer[MAX_HTTP_REQUEST_SIZE] = {0};

        // Read the incoming request
        int valread = read(new_socket, buffer, sizeof(buffer) - 1);
        if (valread < 0)
        {
            perror("❌ read failed");
            close(new_socket);
            exit(EXIT_FAILURE);
        }

        HttpRequest *parsed_http_req = parse_http_request(buffer);

        if (parsed_http_req == NULL)
        {
            perror("❌ req pars failed");
            close(new_socket);
            exit(EXIT_FAILURE);
        }

        print_http_request(parsed_http_req);
        HttpResponse *serv_res = handle_request(parsed_http_req);
        print_http_response(serv_res);
        // const char *http_response = build_http_response(&parsed_http_req);
        // if (http_response == NULL)
        // {
        //     perror("❌ res build failed");
        //     close(new_socket);
        //     exit(EXIT_FAILURE);
        // }

        // write(new_socket, http_response, strlen(http_response));
        // printf("Response sent.\n");

        // Close the new socket
        close(new_socket);
    }
    // Close the server socket (after done listening)
    close(server_fd);
    return 0;
}

HttpResponse *handle_request(HttpRequest *req)
{
    if (req == NULL)
    {
        return make_error_response(500, "req null", "");
    }

    char file_path[MAX_PATH_SIZE];

    // Remove leading slash from path if necessary
    char *path = req->path;
    if (path[0] != '/')
    {
        return make_error_response(500, "path mus start wich leading '/'", "");
    }

    // Build file path string
    snprintf(file_path, sizeof(file_path), "z_server_files%s.json", path); // server/...

    if (strcmp(req->method, "GET") == 0)
    {
        return handle_get(req, file_path);
    }
    else if (strcmp(req->method, "POST") == 0)
    {
        return handle_post(req, file_path);
    }
    else if (strcmp(req->method, "PUT") == 0)
    {
        return handle_put(req, file_path);
    }
    else if (strcmp(req->method, "DELETE") == 0)
    {
        return handle_delete(req, file_path);
    }
    else
    {
        return make_error_response(500, "Unknown Method", "");
    }
    return make_error_response(500, "unknown server errror", "");
}

HttpResponse *handle_post(HttpRequest *req, char *file_path)
{
    const char *new_content = req->body;

    FILE *file = fopen(file_path, "w");
    if (file == NULL)
    {
        perror("Could not open file for writing");

        HttpResponse *res = malloc(sizeof(HttpResponse));
        *res = (HttpResponse){
            .version = "HTTP/1.1",
            .status_code = 500,
            .status_text = "Internal Server Error",
            .header_count = 0};
        snprintf(res->body, MAX_BODY_SIZE, "Could not open file for writing");
        return res;
    }

    fprintf(file, "%s", new_content);
    fclose(file);

    HttpResponse *res = malloc(sizeof(HttpResponse));
    *res = (HttpResponse){
        .version = "HTTP/1.1",
        .status_code = 200,
        .status_text = "OK",
        .header_count = 0};
    snprintf(res->body, MAX_BODY_SIZE, "Data successfully written.");
    return res;
}

HttpResponse *handle_get(HttpRequest *req, char *file_path)
{
    HttpResponse *res = malloc(sizeof(HttpResponse));
    *res = (HttpResponse){
        .version = "HTTP/1.1",
        .status_code = 200,
        .status_text = "OK",
        .header_count = 0};
    snprintf(res->body, MAX_BODY_SIZE, "todo");
    return res;
}

HttpResponse *handle_delete(HttpRequest *req, char *file_path)
{
    HttpResponse *res = malloc(sizeof(HttpResponse));
    *res = (HttpResponse){
        .version = "HTTP/1.1",
        .status_code = 200,
        .status_text = "OK",
        .header_count = 0};
    snprintf(res->body, MAX_BODY_SIZE, "todo");
    return res;
}

HttpResponse *handle_put(HttpRequest *req, char *file_path)
{
    HttpResponse *res = malloc(sizeof(HttpResponse));
    *res = (HttpResponse){
        .version = "HTTP/1.1",
        .status_code = 200,
        .status_text = "OK",
        .header_count = 0};
    snprintf(res->body, MAX_BODY_SIZE, "todo");
    return res;
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
