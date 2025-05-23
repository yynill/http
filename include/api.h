#ifndef API_H
#define API_H

#include "http.h"
#include "db.h"


HttpResponse *handle_get(HttpRequest *req, sqlite3 **db);
HttpResponse *handle_delete(HttpRequest *req, sqlite3 **db);
HttpResponse *handle_post(HttpRequest *req, sqlite3 **db);
HttpResponse *handle_put(HttpRequest *req, sqlite3 **db);

#endif // API_H
