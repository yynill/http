
#include "../include/api.h"

HttpResponse *handle_get(HttpRequest *req)
{
    HttpResponse *res = malloc(sizeof(HttpResponse));
    *res = (HttpResponse){
        .version = "HTTP/1.1",
        .status_code = 200,
        .status_text = "OK",
        .header_count = 0};
    snprintf(req->body, res->body_size, "todo");
    return res;
}

HttpResponse *handle_delete(HttpRequest *req)
{
    HttpResponse *res = malloc(sizeof(HttpResponse));
    *res = (HttpResponse){
        .version = "HTTP/1.1",
        .status_code = 200,
        .status_text = "OK",
        .header_count = 0};
    snprintf(req->body, res->body_size, "todo");
    return res;
}

HttpResponse *handle_put(HttpRequest *req)
{
    HttpResponse *res = malloc(sizeof(HttpResponse));
    *res = (HttpResponse){
        .version = "HTTP/1.1",
        .status_code = 200,
        .status_text = "OK",
        .header_count = 0};
    snprintf(req->body, res->body_size, "todo");
    return res;
}

HttpResponse *handle_post(HttpRequest *req)
{
    HttpResponse *res = malloc(sizeof(HttpResponse));
    *res = (HttpResponse){
        .version = "HTTP/1.1",
        .status_code = 200,
        .status_text = "OK",
        .header_count = 0};
    snprintf(req->body, res->body_size, "todo");
    return res;
}