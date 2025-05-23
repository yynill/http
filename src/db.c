#include <stdio.h>
#include "../include/db.h"

int db_open(sqlite3 **db) {
    int rc = sqlite3_open("./z_server_files/database.db", db);
    if (rc) {
        fprintf(stderr, "❌ cant open db: %s\n", sqlite3_errmsg(*db));
        return rc;
    }
    return SQLITE_OK;
}

int create_user_table(sqlite3 *db) {
    const char *sql = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "age INTEGER NOT NULL"
        ");";

    return db_exec(db, sql);
}

void print_all_users(sqlite3 *db) {
    const char *sql = "SELECT * FROM users;";
    int rc = db_query(db, sql, print_callback);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ Failed to query users\n");
    }
}


void db_close(sqlite3 *db) {
    sqlite3_close(db);
}

int db_exec(sqlite3 *db, const char *sql) {
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
    return rc;
}

int db_query(sqlite3 *db, const char *sql, int (*callback)(void*, int, char**, char**)) {
    char *errMsg = 0;
    int rc = sqlite3_exec(db, sql, callback, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "❌ SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
    }
    return rc;
}

int print_callback(void *NotUsed, int argc, char **argv, char **azColName) {
    for (int i = 0; i < argc; i++) {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void append_to_json(JsonBuffer *buff, const char *str) {
    int len = strlen(str);
    if (buff->capacity < buff->size + len)
    {
        if (buff->capacity == 0) buff->capacity = 128;
        while (buff->capacity < buff->size + len)
            buff->capacity *= 2;
        buff->data = realloc(buff->data, buff->capacity);
    }
    memcpy(buff->data + buff->size, str, len);
    buff->size += len;
}

int json_callback(void *buffer, int argc, char **argv, char **azColName) {
    JsonBuffer *buf = (JsonBuffer *)buffer;

    if (buf->size == 1) append_to_json(buf, "{");
    else append_to_json(buf, ", {");

    for (int i = 0; i < argc; i++) {
        append_to_json(buf, "\"");
        append_to_json(buf, azColName[i]);
        append_to_json(buf, "\": \"");
        append_to_json(buf, argv[i] ? argv[i] : "NULL");
        append_to_json(buf, "\"");

        if (i < argc - 1)
            append_to_json(buf, ", ");
    }

    append_to_json(buf, "}");
    return 0;
}

JsonBuffer *db_query_json(sqlite3 *db, const char *sql)
{
    char *errMsg = 0;
    JsonBuffer *buffer = malloc(sizeof(JsonBuffer));
    if (!buffer) return NULL;
    buffer->data = NULL;
    buffer->size = 0;
    buffer->capacity = 0;

    append_to_json(buffer, "[");

    int rc = sqlite3_exec(db, sql, json_callback, buffer, &errMsg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "❌ SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        free(buffer);
        return NULL;
    }

    append_to_json(buffer, "]");

    return buffer;
}
