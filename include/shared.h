#ifndef SHARED_H
#define SHARED_H

#define PORT 8080

#include "http.h"
#include "db.h"

// server functions
int start_server(sqlite3 **db);
int send_respose(HttpResponse *res,int socket);

HttpResponse *handle_request(HttpRequest *req, sqlite3 **db);
HttpResponse *handle_file_request(char *file_path);

// client functions
int run_client();

#endif // SHARED_H