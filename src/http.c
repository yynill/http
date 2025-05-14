#include "../include/http.h"

char *build_http_request(HttpRequest *req)
{
    // Calculate the size needed for the request
    size_t size = strlen(req->method) + strlen(req->path) + strlen(req->host) + 100; // Extra space for headers and formatting
    size_t body_length = 0;

    if (req->body[0] != '\0') // Check if body is not empty
    {
        body_length = strlen(req->body); // Get the length of the body
        size += body_length;             // Add the body length to the total size
    }

    // Allocate memory for request
    char *request = (char *)malloc(size);
    if (!request)
    {
        return NULL;
    }

    // Build the HTTP request
    // ---------------------
    // POST / HTTP/1.1\r\n
    // Host: localhost\r\n
    // \r\n
    // "Content-Length: 13"
    // \r\n
    // Hello, World!
    snprintf(request, size,
             "%s %s %s\r\n"
             "Host: %s\r\n",
             req->method, req->path, req->version, req->host);

    // Add body if provided
    if (req->body[0] != '\0' && body_length > 0) // Check if body is not empty
    {
        // Create Content-Length header dynamically based on body length
        char content_length[64];
        snprintf(content_length, sizeof(content_length),
                 "Content-Length: %zu\r\n\r\n",
                 body_length);

        // Append Content-Length header to the request
        strncat(request, content_length, size - strlen(request) - 1);

        // Append the actual body to the request
        strncat(request, req->body, size - strlen(request) - 1);
    }
    else
    {
        strncat(request, "\r\n", size - strlen(request) - 1);
    }

    return request;
}

HttpRequest *parse_http_request(const char *request_str)
{
    HttpRequest *req = malloc(sizeof(HttpRequest));
    if (!req)
        return NULL;
    memset(req, 0, sizeof(HttpRequest));

    // Parse the request line (e.g., POST /path HTTP/1.1)\r\n
    const char *line_end = strstr(request_str, "\r\n");
    if (!line_end)
        return req;
    char request_line[512] = {0};
    size_t line_len = line_end - request_str;
    if (line_len >= sizeof(request_line))
        line_len = sizeof(request_line) - 1;
    strncpy(request_line, request_str, line_len);

    // Parse method, path, version
    sscanf(request_line, "%7s %255s %15s", req->method, req->path, req->version);

    // Find Host header
    const char *host_hdr = strstr(request_str, "Host: ");
    if (host_hdr)
    {
        host_hdr += 6; // skip "Host: "
        const char *host_end = strstr(host_hdr, "\r\n");
        if (host_end)
        {
            size_t host_len = host_end - host_hdr;
            if (host_len >= MAX_HOST_SIZE)
                host_len = MAX_HOST_SIZE - 1;
            strncpy(req->host, host_hdr, host_len);
            req->host[host_len] = '\0';
        }
    }

    // Only look for a body if method is not GET or DELETE
    if (strcmp(req->method, "GET") != 0 && strcmp(req->method, "DELETE") != 0)
    {
        // Find the end of headers ("\r\n\r\n")
        const char *body_start = strstr(request_str, "\r\n\r\n");
        if (body_start)
        {
            body_start += 4; // skip past "\r\n\r\n"
            size_t body_len = strlen(body_start);
            if (body_len >= MAX_BODY_SIZE)
                body_len = MAX_BODY_SIZE - 1;
            strncpy(req->body, body_start, body_len);
            req->body[body_len] = '\0';
        }
    }

    return req;
}

char *build_http_response(HttpRequest *req)
{
    return NULL;
}