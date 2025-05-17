#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


#define MAX_METHOD_SIZE 8
#define MAX_VERSION_SIZE 16
#define MAX_PATH_SIZE 256
#define MAX_HOST_SIZE 256
#define MAX_BODY_SIZE 2048
#define MAX_HEADERS 32

#define MAX_HTTP_REQUEST_SIZE 4096 // 4KB buffer

typedef struct
{
    char *name;  // Header name (e.g., "HOST", "Content-Type")
    char *value; // Header value (e.g., "localhost", "text/html")
} Header;

typedef struct
{
    char method[MAX_METHOD_SIZE];   // GET, POST, PUT, DELETE, etc.
    char path[MAX_PATH_SIZE];       // URL path (e.g., /index.html)
    char version[MAX_VERSION_SIZE]; // eg. HTTP/1.1
    int header_count;               // num how many headers in req;
    Header headers[MAX_HEADERS];    // header
    char body[MAX_BODY_SIZE];       // Request body (for POST, PUT)
} HttpRequest;

typedef struct
{
    char version[MAX_VERSION_SIZE]; // e.g., "HTTP/1.1"
    int status_code;                // e.g., 200, 404, 500
    char status_text[64];           // e.g., "OK", "Not Found"
    int header_count;               // Number of headers
    Header headers[MAX_HEADERS];    // header
    char body[MAX_BODY_SIZE];       // Response body
} HttpResponse;

// functions
void add_req_header(HttpRequest *req, char *name, char *value);
void add_res_header(HttpResponse *res, char *name, char *value);

char *build_http_request(HttpRequest *req);
void free_http_request(HttpRequest *req);
HttpRequest *parse_http_request(char *request_str);
void print_http_request(HttpRequest *req);

char *build_http_response(HttpResponse *res);
void free_http_response(HttpResponse *res);
void print_http_response(HttpResponse *res);
HttpResponse *make_error_response(int status_code, const char *status_text, const char *body_message);

#endif // HTTP_H