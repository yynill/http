#ifndef SHARED_H
#define SHARED_H

#define PORT 8080

#include "http.h"

// server functions
int start_server();
int send_respose(HttpResponse *res,int socket);

HttpResponse *handle_request(HttpRequest *req);
HttpResponse *handle_file_request(char *file_path);

// client functions
int run_client();

#endif // SHARED_H