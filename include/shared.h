#ifndef SHARED_H
#define SHARED_H

#define PORT 8080

#include "http.h"

// server functions
int start_server();

HttpResponse *handle_request(HttpRequest *req);
HttpResponse *handle_get(char *file_path);
HttpResponse *handle_delete(HttpRequest *req, char *file_path);
HttpResponse *handle_post(HttpRequest *req, char *file_path);
HttpResponse *handle_put(HttpRequest *req, char *file_path);

// client functions
int run_client();

#endif // SHARED_H