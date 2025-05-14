#ifndef HTTP_H
#define HTTP_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEADER_SIZE 512

#define MAX_METHOD_SIZE 8
#define MAX_VERSION_SIZE 16
#define MAX_PATH_SIZE 256
#define MAX_HOST_SIZE 256
#define MAX_BODY_SIZE 2048
#define MAX_HEADERS 32
#define MAX_HEADER_KEY_SIZE 64
#define MAX_HEADER_VALUE_SIZE 256

typedef struct
{
    char method[MAX_METHOD_SIZE];   // GET, POST, PUT, DELETE, etc.
    char path[MAX_PATH_SIZE];       // URL path (e.g., /index.html)
    char version[MAX_VERSION_SIZE]; // eg. HTTP/1.1
    char host[MAX_HOST_SIZE];       // e.g., localhost
    char body[MAX_BODY_SIZE];       // Request body (for POST, PUT)
} HttpRequest;

// TODO: add proper headers

// functions
char *build_http_request(HttpRequest *req);
HttpRequest *parse_http_request(const char *request_str);
char *build_http_response(HttpRequest *req);

#endif // HTTP_H