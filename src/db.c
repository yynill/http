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
