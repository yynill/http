#include "../include/api.h"
#include "../include/db.h"

HttpResponse *handle_get(HttpRequest *req, sqlite3 **db)
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

HttpResponse *handle_delete(HttpRequest *req, sqlite3 **db)
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

HttpResponse *handle_put(HttpRequest *req, sqlite3 **db)
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

HttpResponse *handle_post(HttpRequest *req, sqlite3 **db)
{
    if (strncmp(req->path, "/api/add_entry", 14) == 0)
    {
        // name=[somename]&age=[some age]
        char name[100];
        char age[10];
        sscanf(req->body, "name=%99[^&]&age=%9s", name, age);

        int num = atoi(age);

        char sql[256];
        snprintf(sql, sizeof(sql), "INSERT INTO users (name, age) VALUES ('%s', %d);", name, num);
        printf("🟠 %s \n", sql);

        if (db_exec(*db, sql) != SQLITE_OK) return make_error_response(500, "Could not write to db", "");

        // int sucess = db_exec(&db, );
        HttpResponse *res = malloc(sizeof(HttpResponse));
        *res = (HttpResponse){
            .version = "HTTP/1.1",
            .status_code = 200,
            .status_text = "OK",
            .header_count = 0,
            .body = "",
        };

        add_res_header(res, "Location", "index.html");
        add_res_header(res, "Content-Length", "0");
        return res;
    }

    return make_error_response(500, "Could not write to db", "");
}