#ifndef DB_H
#define DB_H

#include <sqlite3.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char *data;
    int size;
    int capacity;
} JsonBuffer;

int db_open(sqlite3 **db);
void db_close(sqlite3 *db);

int create_user_table(sqlite3 *db);
void print_all_users(sqlite3 *db);

int db_exec(sqlite3 *db, const char *sql);                                                  // execute something (CREATE, INSERT)
int db_query(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **)); // execute something with Callback
int print_callback(void *NotUsed, int argc, char **argv, char **azColName);
JsonBuffer *db_query_json(sqlite3 *db, const char *sql);


#endif
