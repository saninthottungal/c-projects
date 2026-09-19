#ifndef DB_H
#define DB_H

#include <sqlite3.h>

// Connects to the SQLite database and returns the connection handle.
// Enforces foreign key constraints.
sqlite3* db_connect(const char* db_file);

// Closes the SQLite database connection safely.
void db_close(sqlite3* db);

// Helper function to execute a simple SQL command (e.g. PRAGMA, INSERT, CREATE TABLE).
int db_execute(sqlite3* db, const char* sql);

// Initializes the database tables if they do not exist.
int db_initialize_schema(sqlite3* db);

#endif // DB_H
