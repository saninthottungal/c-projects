#include "db.h"
#include <stdio.h>
#include <stdlib.h>

sqlite3* db_connect(const char* db_file) {
    sqlite3* db = NULL;
    int rc = sqlite3_open(db_file, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return NULL;
    }

    // Enable foreign keys constraints
    rc = db_execute(db, "PRAGMA foreign_keys = ON;");
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to enable foreign key support.\n");
        sqlite3_close(db);
        return NULL;
    }

    return db;
}

void db_close(sqlite3* db) {
    if (db) {
        sqlite3_close(db);
    }
}

int db_execute(sqlite3* db, const char* sql) {
    char* err_msg = NULL;
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", err_msg);
        sqlite3_free(err_msg);
    }
    return rc;
}

int db_initialize_schema(sqlite3* db) {
    if (!db) return SQLITE_ERROR;

    // Create books table
    const char* sql_books = 
        "CREATE TABLE IF NOT EXISTS books ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  title TEXT NOT NULL,"
        "  author TEXT NOT NULL,"
        "  isbn TEXT UNIQUE NOT NULL,"
        "  quantity INTEGER NOT NULL DEFAULT 1,"
        "  available INTEGER NOT NULL DEFAULT 1"
        ");";

    // Create members table
    const char* sql_members = 
        "CREATE TABLE IF NOT EXISTS members ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  name TEXT NOT NULL,"
        "  email TEXT UNIQUE NOT NULL,"
        "  phone TEXT,"
        "  join_date TEXT NOT NULL"
        ");";

    // Create loans table
    const char* sql_loans = 
        "CREATE TABLE IF NOT EXISTS loans ("
        "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "  book_id INTEGER NOT NULL,"
        "  member_id INTEGER NOT NULL,"
        "  borrow_date TEXT NOT NULL,"
        "  due_date TEXT NOT NULL,"
        "  return_date TEXT,"
        "  FOREIGN KEY (book_id) REFERENCES books(id) ON DELETE RESTRICT,"
        "  FOREIGN KEY (member_id) REFERENCES members(id) ON DELETE RESTRICT"
        ");";

    int rc = db_execute(db, sql_books);
    if (rc != SQLITE_OK) return rc;

    rc = db_execute(db, sql_members);
    if (rc != SQLITE_OK) return rc;

    rc = db_execute(db, sql_loans);
    return rc;
}
