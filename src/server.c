#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "../include/shared.h"
#include "../include/http.h"
#include "../include/db.h"
#include "../include/api.h"

int server_fd;

// server won't leave open sockets behind when Ctrl+C out
void handle_sigint()
{
    printf("\nCaught SIGINT, shutting down server.\n");
    close(server_fd);
    exit(0);
}

int start_server(sqlite3 **db)
{
    printf("Starting HTTP server...\n");

    // Create the socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == 0)
    {
        perror("❌ socket failure");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        perror("❌ setsockopt failed");
        exit(EXIT_FAILURE);
    }
    printf("Socket created.\n");

    // Bind socket to an IP address and port
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

    // Listen for incoming connections
    if (listen(server_fd, 3) < 0)
    {
        perror("❌listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Listening for incoming connections...\n\n");

    // server loop
    while (1)
    {
        // accept incoming connections
        int new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            perror("❌ accept failed");
            exit(EXIT_FAILURE);
        }

        char buffer[MAX_HTTP_REQUEST_SIZE] = {0};
        int valread = read(new_socket, buffer, sizeof(buffer) - 1);
        if (valread < 0)
        {
            perror("❌ read failed");
            close(new_socket);
            exit(EXIT_FAILURE);
        }

        printf("%s\n\n", buffer);
        HttpRequest *parsed_http_req = parse_http_request(buffer);

        if (parsed_http_req == NULL)
        {
            perror("❌ req pars failed");
            close(new_socket);
            exit(EXIT_FAILURE);
        }

        // print_http_request(parsed_http_req);
        HttpResponse *res = handle_request(parsed_http_req, db);
        // print_http_response(res);

        int success = send_respose(res, new_socket);
        if (success == 0)
        {
            perror("❌ res send failed");
            close(new_socket);
            exit(EXIT_FAILURE);
        }

        close(new_socket);
    }
    close(server_fd);
    return 0;
}

int send_respose(HttpResponse *res, int socket)
{
    if (res == NULL) return 0;
    char *http_response;
    if (strncmp(res->body_mime, "text/", 5) == 0)
    {
        http_response = build_http_response(res, 1);
        if (http_response == NULL)
        return 0;
        write(socket, http_response, strlen(http_response));
    }
    else if (strncmp(res->body_mime, "image/", 6) == 0)
    {
        http_response = build_http_response(res, 0);
        if (http_response == NULL) return 0;
        write(socket, http_response, strlen(http_response));
        write(socket, res->body, res->body_size);
    }
    else if (strncmp(res->body_mime, "application/", 12) == 0)
    {
        http_response = build_http_response(res, 1);
        if (http_response == NULL)  return 0;
        write(socket, http_response, strlen(http_response));
        write(socket, res->body, res->body_size);
    }
    else 
    {
        http_response = build_http_response(res, 1);
        if (http_response == NULL) return 0;
        write(socket, http_response, strlen(http_response));
    }
    
    printf("Response print:\n%s\n", http_response);
    printf("Response sent.\n\n");

    return 1;
}

HttpResponse *handle_request(HttpRequest *req, sqlite3 **db)
{
    if (req == NULL)
    {
        return make_error_response(500, "req null", "");
    }

    if (req->path[0] != '/')
    {
        return make_error_response(500, "path mus start wich leading '/'", "");
    }

    if (strncmp(req->path, "/api/", 5) == 0)
    {
        if (strcmp(req->method, "GET") == 0) return handle_get(req, db);
        if (strcmp(req->method, "POST") == 0) return handle_post(req, db);
        if (strcmp(req->method, "PUT") == 0) return handle_put(req, db);
        if (strcmp(req->method, "DELETE") == 0) return handle_delete(req, db);

        return make_error_response(500, "Unknown Method", "");
    }
    else
    {
        char file_path[MAX_PATH_SIZE];
        snprintf(file_path, sizeof(file_path), "z_server_files%s", req->path); // server/...
        return handle_file_request(file_path);
    }
}

int file_exists(const char *path)
{
    FILE *file = fopen(path, "r");
    if (file)
    {
        fclose(file);
        return 1;
    }
    return 0;
}

HttpResponse *handle_file_request(char *file_path)
{
    if (!file_exists(file_path))
        return make_error_response(404, "File not found", "");

    HttpResponse *res = malloc(sizeof(HttpResponse));
    if (!res)
    {
        perror("malloc failed");
        exit(1);
    }

    *res = (HttpResponse){
        .version = "HTTP/1.1",
        .status_code = 200,
        .status_text = "OK",
        .header_count = 0,
        .body = NULL,
        .body_size = 0,
    };
    strcpy(res->body_mime, get_mime_type(file_path));

    const char *open_mode = (strncmp(res->body_mime, "text/", 5) == 0) ? "r" : "rb";

    FILE *file = fopen(file_path, open_mode);
    if (!file)
    {
        return make_error_response(500, "Could not open file for reading", "");
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    int filesize = ftell(file);
    rewind(file);

    res->body_size = filesize;
    res->body = malloc(res->body_size + 1); // +1 for '\0' terminator
    if (!res->body)
    {
        perror("malloc failed");
        fclose(file);
        exit(1);
    }

    int bytes_read = fread(res->body, 1, res->body_size, file);
    fclose(file);

    if (strncmp(res->body_mime, "text/", 5) == 0)
    {
        res->body[bytes_read] = '\0';
    }

    return res;
}

int main()
{
    sqlite3 *db;

    if (db_open(&db) != SQLITE_OK) return 1;
    
    create_user_table(db);
    // print_all_users(db);

    signal(SIGINT, handle_sigint);

    if (start_server(&db) < 0)
    {
        fprintf(stderr, "Failed to start server\n");
        return 1;
    }
    db_close(db);
    return 0;
}
