#ifndef DB_H
#define DB_H

#include <sqlite3.h>

int db_open(sqlite3 **db);
void db_close(sqlite3 *db);
int print_callback(void *NotUsed, int argc, char **argv, char **azColName);
int db_exec(sqlite3 *db, const char *sql);                                                  // execute something (CREATE, INSERT)
int db_query(sqlite3 *db, const char *sql, int (*callback)(void *, int, char **, char **)); // execute something with Callback

#endif
