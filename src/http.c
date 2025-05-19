#include "../include/http.h"

void add_req_header(HttpRequest *req, char *name, char *value)
{
    if (req->header_count >= MAX_HEADERS)
    {
        printf("Header limit reached.\n");
        return;
    }
    if (name == NULL || value == NULL)
    {
        printf("Invalid header.\n");
        return;
    }

    req->headers[req->header_count].name = strdup(name);
    req->headers[req->header_count].value = strdup(value);
    req->header_count++;
    return;
}

void add_res_header(HttpResponse *res, char *name, char *value)
{
    if (res->header_count >= MAX_HEADERS)
    {
        printf("Header limit reached.\n");
        return;
    }
    if (name == NULL || value == NULL)
    {
        printf("Invalid header.\n");
        return;
    }

    res->headers[res->header_count].name = strdup(name);
    res->headers[res->header_count].value = strdup(value);
    res->header_count++;
    return;
}

// Build the HTTP request
// ---------------------
// POST / HTTP/1.1\r\n             | Request line
// Host: localhost\r\n             | Headers
// Content-Length: 13\r\n          |
// Content-Type: text/plain\r\n    |
// \r\n                            |
// Hello, World!                   | Body
char *build_http_request(HttpRequest *req)
{
    // Calculate the size needed for the request
    size_t size = 0;
    size += strlen(req->method) + 1 + strlen(req->path) + 1 + strlen(req->version) + 2; // \r\n
    for (int i = 0; i < req->header_count; i++)
    {
        size += strlen(req->headers[i].name) + 2 + strlen(req->headers[i].value) + 2; // "Name: Value\r\n"
    }
    size += 2; // blank line after headers
    size += strlen(req->body);
    size += 1; // null terminator

    if (size > MAX_HTTP_REQUEST_SIZE)
    {
        printf("reqest to large");
        return NULL;
    }

    // Allocate memory for request
    char *request = (char *)malloc(size);
    if (!request)
    {
        return NULL;
    }

    // request line
    size_t offset = 0;
    offset += snprintf(request + offset, size - offset, "%s %s %s\r\n", req->method, req->path, req->version);

    // Headers
    for (int i = 0; i < req->header_count; i++)
    {
        offset += snprintf(request + offset, size - offset, "%s: %s\r\n", req->headers[i].name, req->headers[i].value);
    }
    offset += snprintf(request + offset, size - offset, "\r\n");

    // body
    if (strlen(req->body) > 0)
    {
        strncat(request + offset, req->body, size - offset - 1);
    }
    else
    {
        strncat(request + offset, "\r\n", size - offset - 1);
    }

    return request;
}

void free_http_request(HttpRequest *req)
{
    for (int i = 0; i < req->header_count; ++i)
    {
        free(req->headers[i].name);
        free(req->headers[i].value);
    }
    return;
}

HttpRequest *parse_http_request(char *request_str)
{
    HttpRequest *req = malloc(sizeof(HttpRequest));
    if (!req)
        return NULL;
    req->header_count = 0;

    char *pos = request_str; // pointer traverse through that string (set to string start)

    // Parse request line: METHOD PATH VERSION\r\n
    sscanf(pos, "%s %s %s\r\n", req->method, req->path, req->version);
    pos = strstr(pos, "\r\n") + 2;

    // Parse headers
    while (pos && !(pos[0] == '\r' && pos[1] == '\n'))
    {
        if (req->header_count >= MAX_HEADERS)
        {
            printf("Header limit reached\n");
            break;
        }

        char name[128], value[512];

        sscanf(pos, "%[^:]: %[^\r\n]\r\n", name, value);

        add_req_header(req, name, value);

        pos = strstr(pos, "\r\n") + 2; // add len of line to position
    }

    // Skip the blank line (\r\n) before body
    pos += 2;

    // Only look for a body if method is POST, PUT
    if (strcmp(req->method, "POST") == 0 || strcmp(req->method, "PUT") == 0)
    {
        // Copy body
        strncpy(req->body, pos, MAX_BODY_SIZE - 1);
        req->body[MAX_BODY_SIZE - 1] = '\0';
    }

    return req;
}

void print_http_request(HttpRequest *req)
{
    if (req == NULL)
    {
        printf("Failed to print req");
        return;
    }

    printf("\nRequest print\n\n");
    printf("method: %s\n", req->method);
    printf("path: %s\n", req->path);
    printf("version: %s\n", req->version);
    printf("\n(%d) Headers:\n\n", req->header_count);
    int count = req->header_count;
    for (int i = 0; i < count; i++)
    {
        printf("%s: %s\n", req->headers[i].name, req->headers[i].value);
    }
    printf("\nbody: %s\n\n", req->body);
}

// http responde format
// ------------------------------------------------
// HTTP/1.1 200 OK\r\n                          | Status line
// Content-Type: text/html; charset=UTF-8\r\n   | Headers
// Content-Length: 70\r\n                       | 
// \r\n                                         |
// <html>\r\n                                   | Body
//   <head><title>Example</title></head>\r\n    |
//   <body>Hello World</body>\r\n               |
// </html>\r\n                                  |
char *build_http_response(HttpResponse *res)
{
   // calculate size
    size_t size = 0; 
    size += 3 + 1 + strlen(res->status_text) + 1 + strlen(res->version) + 2; // sttus code always 3 long  - \r\n 2 long
    for (int i = 0; i < res->header_count; i++)
    {
        size += strlen(res->headers[i].name) + 2 + strlen(res->headers[i].value) + 2;
    }
    size += 2; // blank line after headers
    size += strlen(res->body);
    size += 1; // null terminator

    // allocate mem
    char *response = (char *)malloc(size);
    if (!response){
        return NULL;
    }

    size_t offset = 0;
    // status line 
    offset += snprintf(response + offset, size - offset, "%s %d %s\r\n", res->version, res->status_code, res->status_text);

    // Headers
    for (int i = 0; i < res->header_count; i++)
    {
        offset += snprintf(response + offset, size - offset, "%s: %s\r\n", res->headers[i].name, res->headers[i].value);
    }
    offset += snprintf(response + offset, size - offset, "\r\n");

        // body
    if (strlen(res->body) > 0)
    {
        strncat(response + offset, res->body, size - offset - 1);
    }
    else
    {
        strncat(response + offset, "\r\n", size - offset - 1);
    }

    return response;
}

void free_http_response(HttpResponse *res)
{
    return;
}

HttpResponse *make_error_response(int status_code, const char *status_text, const char *body_message)
{
    HttpResponse *res = malloc(sizeof(HttpResponse));
    if (res == NULL)
    {
        perror("Failed to allocate HttpResponse");
        exit(1);
    }
    *res = (HttpResponse){
        .version = "HTTP/1.1",
        .status_code = status_code,
        .header_count = 0};
    strncpy(res->status_text, status_text, sizeof(res->status_text) - 1);
    res->status_text[sizeof(res->status_text) - 1] = '\0';
    snprintf(res->body, MAX_BODY_SIZE, "%s", body_message);
    return res;
}

void print_http_response(HttpResponse *res)
{
    if (res == NULL)
    {
        printf("Failed to print res");
        return;
    }

    printf("\nResponse print\n\n");
    printf("version: %s\n", res->version);
    printf("status code: %d\n", res->status_code);
    printf("status text: %s\n", res->status_text);
    printf("\n(%d) Headers:\n\n", res->header_count);
    int count = res->header_count;
    for (int i = 0; i < count; i++)
    {
        printf("%s: %s\n", res->headers[i].name, res->headers[i].value);
    }
    printf("body: %s\n\n", res->body);
}

// mime stands for Multipurpose Internet Mail Extensions
char *get_mime_type(char *path) {
    // Find the last dot in the path
    char *ext = strrchr(path, '.');
    if (!ext) {
        return "application/octet-stream";
    }

    // Compare extension and return appropriate MIME type
    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".htm") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".txt") == 0) return "text/plain";
    if (strcmp(ext, ".js") == 0) return "application/javascript";
    if (strcmp(ext, ".json") == 0) return "application/json";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0) return "image/jpeg";
    if (strcmp(ext, ".jpeg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    if (strcmp(ext, ".svg") == 0) return "image/svg+xml";
    if (strcmp(ext, ".webp") == 0) return "image/webp";
    if (strcmp(ext, ".ico") == 0) return "image/x-icon";
    if (strcmp(ext, ".mp3") == 0) return "audio/mpeg";
    if (strcmp(ext, ".wav") == 0) return "audio/wav";
    if (strcmp(ext, ".mp4") == 0) return "video/mp4";
    if (strcmp(ext, ".pdf") == 0) return "application/pdf";

    return "application/octet-stream";
}
