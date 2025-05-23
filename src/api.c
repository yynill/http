#include "../include/api.h"
#include "../include/db.h"

HttpResponse *handle_get(HttpRequest *req, sqlite3 **db)
{
    if (strncmp(req->path, "/api/entries", 12) == 0)
    {
        JsonBuffer *json_result = db_query_json(*db, "SELECT * FROM users;");
        if (!json_result || json_result->size <= 0)
        {
            if (json_result)
                free(json_result);
            return make_error_response(500, "Could not write to db", "");
        }
        json_result->data[json_result->size] = '\0'; // \0

        HttpResponse *res = malloc(sizeof(HttpResponse));
        *res = (HttpResponse){
            .version = "HTTP/1.1",
            .status_code = 200,
            .status_text = "OK",
            .header_count = 0,
            .body = json_result->data,
            .body_size = json_result->size,
            .body_mime = "application/json",
        };

        char len[32];
        sprintf(len, "%d", json_result->size);
        add_res_header(res, "Content-Length", len);
        add_res_header(res, "Content-Type", "application/json");
        add_res_header(res, "Location", "index.html");

        return res;
    }

    return make_error_response(500, "Could not write to db", "");
}

HttpResponse *handle_delete(HttpRequest *req, sqlite3 **db)
{
    // /api/entries/5
    if (strncmp(req->path, "/api/entries", 12) == 0)
    {
        char id_str[9];
        sscanf(req->path, "/api/entries/%8[0-9]", id_str);
        int id = atoi(id_str);

        char sql[256];
        snprintf(sql, sizeof(sql), "DELETE FROM users WHERE id=%d;", id);
        if (db_exec(*db, sql) != SQLITE_OK)
            return make_error_response(500, "Could not write to db", "");
        
        HttpResponse *res = malloc(sizeof(HttpResponse));
        *res = (HttpResponse){
            .version = "HTTP/1.1",
            .status_code = 200,
            .status_text = "OK",
            .header_count = 0};
        return res;
    }
    return make_error_response(500, "Could not write to db", "");
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

        if (db_exec(*db, sql) != SQLITE_OK) return make_error_response(500, "Could not write to db", "");

        HttpResponse *res = malloc(sizeof(HttpResponse));
        *res = (HttpResponse){
            .version = "HTTP/1.1",
            .status_code = 303,
            .status_text = "See Other",
            .header_count = 0,
            .body = "",
        };

        add_res_header(res, "Location", "http://localhost:8080/index.html");
        add_res_header(res, "Content-Length", "0");
        return res;
    }

    return make_error_response(500, "Could not write to db", "");
}