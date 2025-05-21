#ifndef API_H
#define API_H

#include "http.h"


HttpResponse *handle_get(HttpRequest *req);
HttpResponse *handle_delete(HttpRequest *req);
HttpResponse *handle_post(HttpRequest *req);
HttpResponse *handle_put(HttpRequest *req);

#endif // API_H
