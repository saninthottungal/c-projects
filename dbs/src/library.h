#ifndef LIBRARY_H
#define LIBRARY_H

#include <sqlite3.h>

// Structs representing our database entities for internal use
typedef struct {
    int id;
    char title[256];
    char author[256];
    char isbn[64];
    int quantity;
    int available;
} Book;

typedef struct {
    int id;
    char name[256];
    char email[256];
    char phone[64];
    char join_date[64];
} Member;

typedef struct {
    int id;
    int book_id;
    int member_id;
    char book_title[256];
    char member_name[256];
    char borrow_date[64];
    char due_date[64];
    char return_date[64];
} Loan;

// --- Book Operations ---
int add_book(sqlite3* db, const char* title, const char* author, const char* isbn, int quantity);
int list_books(sqlite3* db);
int search_books(sqlite3* db, const char* query);
int update_book(sqlite3* db, int id, const char* title, const char* author, const char* isbn, int quantity);
int delete_book(sqlite3* db, int id);

// --- Member Operations ---
int add_member(sqlite3* db, const char* name, const char* email, const char* phone);
int list_members(sqlite3* db);
int search_members(sqlite3* db, const char* query);
int update_member(sqlite3* db, int id, const char* name, const char* email, const char* phone);
int delete_member(sqlite3* db, int id);

// --- Loan Operations ---
int borrow_book(sqlite3* db, int book_id, int member_id, int days_to_due);
int return_book(sqlite3* db, int loan_id);
int list_active_loans(sqlite3* db);
int list_loan_history(sqlite3* db);

#endif // LIBRARY_H
