#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Colors for terminal formatting
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_CYAN    "\033[1;36m"

// Prints a string truncated to width with ellipses if it exceeds, and padded to match the width
static void print_cell(const char* str, int width, int align_left) {
    if (!str) str = "";
    int len = strlen(str);
    if (len <= width) {
        if (align_left) {
            printf("%-*s", width, str);
        } else {
            printf("%*s", width, str);
        }
    } else {
        // Truncate and add "..."
        for (int i = 0; i < width - 3; i++) {
            putchar(str[i]);
        }
        printf("...");
    }
}

// Helper to check database errors and print clean messages
static void print_db_error(sqlite3* db, const char* context) {
    fprintf(stderr, COLOR_RED "Database Error during %s: %s\n" COLOR_RESET, context, sqlite3_errmsg(db));
}

// --- Book Operations ---

int add_book(sqlite3* db, const char* title, const char* author, const char* isbn, int quantity) {
    if (quantity <= 0) {
        printf(COLOR_RED "Error: Quantity must be greater than 0.\n" COLOR_RESET);
        return -1;
    }

    const char* sql = "INSERT INTO books (title, author, isbn, quantity, available) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "add book preparation");
        return -1;
    }

    sqlite3_bind_text(stmt, 1, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, author, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, isbn, -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, quantity);
    sqlite3_bind_int(stmt, 5, quantity); // Available count equals quantity initially

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        printf(COLOR_GREEN "Success: Book added successfully.\n" COLOR_RESET);
        return 0;
    } else if (rc == SQLITE_CONSTRAINT || rc == SQLITE_CONSTRAINT_UNIQUE) {
        printf(COLOR_RED "Error: A book with ISBN '%s' already exists.\n" COLOR_RESET, isbn);
        return -1;
    } else {
        print_db_error(db, "adding book");
        return -1;
    }
}

static void print_book_table_header() {
    printf(COLOR_CYAN);
    printf("┌──────┬──────────────────────────────┬────────────────────────┬────────────────┬───────┬───────┐\n");
    printf("│ ID   │ Title                        │ Author                 │ ISBN           │ Qty   │ Avail │\n");
    printf("├──────┼──────────────────────────────┼────────────────────────┼────────────────┼───────┼───────┤\n");
    printf(COLOR_RESET);
}

static void print_book_table_row(int id, const char* title, const char* author, const char* isbn, int qty, int avail) {
    char id_str[16];
    char qty_str[16];
    char avail_str[16];
    snprintf(id_str, sizeof(id_str), "%d", id);
    snprintf(qty_str, sizeof(qty_str), "%d", qty);
    snprintf(avail_str, sizeof(avail_str), "%d", avail);

    printf("│ ");
    print_cell(id_str, 4, 0);
    printf(" │ ");
    print_cell(title, 28, 1);
    printf(" │ ");
    print_cell(author, 22, 1);
    printf(" │ ");
    print_cell(isbn, 14, 1);
    printf(" │ ");
    print_cell(qty_str, 5, 0);
    printf(" │ ");
    if (avail == 0) {
        printf(COLOR_RED);
    } else if (avail < qty) {
        printf(COLOR_YELLOW);
    } else {
        printf(COLOR_GREEN);
    }
    print_cell(avail_str, 5, 0);
    printf(COLOR_RESET " │\n");
}

static void print_book_table_footer() {
    printf(COLOR_CYAN);
    printf("└──────┴──────────────────────────────┴────────────────────────┴────────────────┴───────┴───────┘\n");
    printf(COLOR_RESET);
}

int list_books(sqlite3* db) {
    const char* sql = "SELECT id, title, author, isbn, quantity, available FROM books ORDER BY id ASC;";
    sqlite3_stmt* stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "list books preparation");
        return -1;
    }

    print_book_table_header();
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* title = (const char*)sqlite3_column_text(stmt, 1);
        const char* author = (const char*)sqlite3_column_text(stmt, 2);
        const char* isbn = (const char*)sqlite3_column_text(stmt, 3);
        int qty = sqlite3_column_int(stmt, 4);
        int avail = sqlite3_column_int(stmt, 5);

        print_book_table_row(id, title, author, isbn, qty, avail);
        count++;
    }
    print_book_table_footer();

    sqlite3_finalize(stmt);
    printf("Total books displayed: %d\n", count);
    return 0;
}

int search_books(sqlite3* db, const char* query) {
    const char* sql = "SELECT id, title, author, isbn, quantity, available FROM books "
                      "WHERE title LIKE ? OR author LIKE ? OR isbn LIKE ? "
                      "ORDER BY id ASC;";
    sqlite3_stmt* stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "search books preparation");
        return -1;
    }

    // Format query search term: %query%
    char search_term[270];
    snprintf(search_term, sizeof(search_term), "%%%s%%", query);

    sqlite3_bind_text(stmt, 1, search_term, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, search_term, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, search_term, -1, SQLITE_TRANSIENT);

    print_book_table_header();
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* title = (const char*)sqlite3_column_text(stmt, 1);
        const char* author = (const char*)sqlite3_column_text(stmt, 2);
        const char* isbn = (const char*)sqlite3_column_text(stmt, 3);
        int qty = sqlite3_column_int(stmt, 4);
        int avail = sqlite3_column_int(stmt, 5);

        print_book_table_row(id, title, author, isbn, qty, avail);
        count++;
    }
    print_book_table_footer();

    sqlite3_finalize(stmt);
    printf("Found %d matching books.\n", count);
    return 0;
}

int update_book(sqlite3* db, int id, const char* title, const char* author, const char* isbn, int quantity) {
    if (quantity <= 0) {
        printf(COLOR_RED "Error: Quantity must be greater than 0.\n" COLOR_RESET);
        return -1;
    }

    // 1. Get the current quantity and available status of this book
    const char* select_sql = "SELECT quantity, available FROM books WHERE id = ?;";
    sqlite3_stmt* select_stmt = NULL;
    int rc = sqlite3_prepare_v2(db, select_sql, -1, &select_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "update book check preparation");
        return -1;
    }

    sqlite3_bind_int(select_stmt, 1, id);
    rc = sqlite3_step(select_stmt);
    if (rc != SQLITE_ROW) {
        printf(COLOR_RED "Error: Book with ID %d not found.\n" COLOR_RESET, id);
        sqlite3_finalize(select_stmt);
        return -1;
    }

    int old_qty = sqlite3_column_int(select_stmt, 0);
    int old_avail = sqlite3_column_int(select_stmt, 1);
    sqlite3_finalize(select_stmt);

    int borrowed = old_qty - old_avail;
    if (quantity < borrowed) {
        printf(COLOR_RED "Error: Cannot set quantity to %d because %d copies are currently borrowed.\n" 
               "Please return copies first before shrinking inventory.\n" COLOR_RESET, quantity, borrowed);
        return -1;
    }

    int new_avail = quantity - borrowed;

    // 2. Perform the update
    const char* update_sql = "UPDATE books SET title = ?, author = ?, isbn = ?, quantity = ?, available = ? WHERE id = ?;";
    sqlite3_stmt* update_stmt = NULL;
    rc = sqlite3_prepare_v2(db, update_sql, -1, &update_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "update book preparation");
        return -1;
    }

    sqlite3_bind_text(update_stmt, 1, title, -1, SQLITE_STATIC);
    sqlite3_bind_text(update_stmt, 2, author, -1, SQLITE_STATIC);
    sqlite3_bind_text(update_stmt, 3, isbn, -1, SQLITE_STATIC);
    sqlite3_bind_int(update_stmt, 4, quantity);
    sqlite3_bind_int(update_stmt, 5, new_avail);
    sqlite3_bind_int(update_stmt, 6, id);

    rc = sqlite3_step(update_stmt);
    sqlite3_finalize(update_stmt);

    if (rc == SQLITE_DONE) {
        printf(COLOR_GREEN "Success: Book details updated successfully.\n" COLOR_RESET);
        return 0;
    } else if (rc == SQLITE_CONSTRAINT || rc == SQLITE_CONSTRAINT_UNIQUE) {
        printf(COLOR_RED "Error: A book with ISBN '%s' already exists.\n" COLOR_RESET, isbn);
        return -1;
    } else {
        print_db_error(db, "updating book");
        return -1;
    }
}

int delete_book(sqlite3* db, int id) {
    // 1. Check if the book is currently borrowed
    const char* check_sql = "SELECT COUNT(*) FROM loans WHERE book_id = ? AND return_date IS NULL;";
    sqlite3_stmt* check_stmt = NULL;
    int rc = sqlite3_prepare_v2(db, check_sql, -1, &check_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "delete book check preparation");
        return -1;
    }

    sqlite3_bind_int(check_stmt, 1, id);
    rc = sqlite3_step(check_stmt);
    int active_loans = 0;
    if (rc == SQLITE_ROW) {
        active_loans = sqlite3_column_int(check_stmt, 0);
    }
    sqlite3_finalize(check_stmt);

    if (active_loans > 0) {
        printf(COLOR_RED "Error: Cannot delete book. There are currently %d unreturned copies.\n" COLOR_RESET, active_loans);
        return -1;
    }

    // 2. Perform the deletion
    const char* delete_sql = "DELETE FROM books WHERE id = ?;";
    sqlite3_stmt* delete_stmt = NULL;
    rc = sqlite3_prepare_v2(db, delete_sql, -1, &delete_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "delete book preparation");
        return -1;
    }

    sqlite3_bind_int(delete_stmt, 1, id);
    rc = sqlite3_step(delete_stmt);
    int affected = sqlite3_changes(db);
    sqlite3_finalize(delete_stmt);

    if (rc == SQLITE_DONE) {
        if (affected > 0) {
            printf(COLOR_GREEN "Success: Book deleted successfully.\n" COLOR_RESET);
            return 0;
        } else {
            printf(COLOR_RED "Error: Book with ID %d not found.\n" COLOR_RESET, id);
            return -1;
        }
    } else if (rc == SQLITE_CONSTRAINT) {
        // Typically foreign key check violation if book has returned loans in history
        printf(COLOR_RED "Error: Cannot delete book because it is referenced in loan history records.\n" 
               "To preserve data integrity, SQLite blocks deletion of this entity.\n" COLOR_RESET);
        return -1;
    } else {
        print_db_error(db, "deleting book");
        return -1;
    }
}


// --- Member Operations ---

int add_member(sqlite3* db, const char* name, const char* email, const char* phone) {
    const char* sql = "INSERT INTO members (name, email, phone, join_date) VALUES (?, ?, ?, DATE('now'));";
    sqlite3_stmt* stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "add member preparation");
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, phone ? phone : "", -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        printf(COLOR_GREEN "Success: Member registered successfully.\n" COLOR_RESET);
        return 0;
    } else if (rc == SQLITE_CONSTRAINT || rc == SQLITE_CONSTRAINT_UNIQUE) {
        printf(COLOR_RED "Error: A member with email '%s' already exists.\n" COLOR_RESET, email);
        return -1;
    } else {
        print_db_error(db, "registering member");
        return -1;
    }
}

static void print_member_table_header() {
    printf(COLOR_CYAN);
    printf("┌──────┬────────────────────────┬──────────────────────────────┬────────────────┬────────────┐\n");
    printf("│ ID   │ Name                   │ Email                        │ Phone          │ Join Date  │\n");
    printf("├──────┼────────────────────────┼──────────────────────────────┼────────────────┼────────────┤\n");
    printf(COLOR_RESET);
}

static void print_member_table_row(int id, const char* name, const char* email, const char* phone, const char* join_date) {
    char id_str[16];
    snprintf(id_str, sizeof(id_str), "%d", id);

    printf("│ ");
    print_cell(id_str, 4, 0);
    printf(" │ ");
    print_cell(name, 22, 1);
    printf(" │ ");
    print_cell(email, 28, 1);
    printf(" │ ");
    print_cell(phone, 14, 1);
    printf(" │ ");
    print_cell(join_date, 10, 1);
    printf(" │\n");
}

static void print_member_table_footer() {
    printf(COLOR_CYAN);
    printf("└──────┴────────────────────────┴──────────────────────────────┴────────────────┴────────────┘\n");
    printf(COLOR_RESET);
}

int list_members(sqlite3* db) {
    const char* sql = "SELECT id, name, email, phone, join_date FROM members ORDER BY id ASC;";
    sqlite3_stmt* stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "list members preparation");
        return -1;
    }

    print_member_table_header();
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        const char* email = (const char*)sqlite3_column_text(stmt, 2);
        const char* phone = (const char*)sqlite3_column_text(stmt, 3);
        const char* join_date = (const char*)sqlite3_column_text(stmt, 4);

        print_member_table_row(id, name, email, phone, join_date);
        count++;
    }
    print_member_table_footer();

    sqlite3_finalize(stmt);
    printf("Total members registered: %d\n", count);
    return 0;
}

int search_members(sqlite3* db, const char* query) {
    const char* sql = "SELECT id, name, email, phone, join_date FROM members "
                      "WHERE name LIKE ? OR email LIKE ? "
                      "ORDER BY id ASC;";
    sqlite3_stmt* stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "search members preparation");
        return -1;
    }

    char search_term[270];
    snprintf(search_term, sizeof(search_term), "%%%s%%", query);

    sqlite3_bind_text(stmt, 1, search_term, -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, search_term, -1, SQLITE_TRANSIENT);

    print_member_table_header();
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* name = (const char*)sqlite3_column_text(stmt, 1);
        const char* email = (const char*)sqlite3_column_text(stmt, 2);
        const char* phone = (const char*)sqlite3_column_text(stmt, 3);
        const char* join_date = (const char*)sqlite3_column_text(stmt, 4);

        print_member_table_row(id, name, email, phone, join_date);
        count++;
    }
    print_member_table_footer();

    sqlite3_finalize(stmt);
    printf("Found %d matching members.\n", count);
    return 0;
}

int update_member(sqlite3* db, int id, const char* name, const char* email, const char* phone) {
    const char* sql = "UPDATE members SET name = ?, email = ?, phone = ? WHERE id = ?;";
    sqlite3_stmt* stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "update member preparation");
        return -1;
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, email, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, phone ? phone : "", -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, id);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc == SQLITE_DONE) {
        int affected = sqlite3_changes(db);
        if (affected > 0) {
            printf(COLOR_GREEN "Success: Member details updated successfully.\n" COLOR_RESET);
            return 0;
        } else {
            printf(COLOR_RED "Error: Member with ID %d not found.\n" COLOR_RESET, id);
            return -1;
        }
    } else if (rc == SQLITE_CONSTRAINT || rc == SQLITE_CONSTRAINT_UNIQUE) {
        printf(COLOR_RED "Error: A member with email '%s' already exists.\n" COLOR_RESET, email);
        return -1;
    } else {
        print_db_error(db, "updating member");
        return -1;
    }
}

int delete_member(sqlite3* db, int id) {
    // 1. Check if the member has active loans
    const char* check_sql = "SELECT COUNT(*) FROM loans WHERE member_id = ? AND return_date IS NULL;";
    sqlite3_stmt* check_stmt = NULL;
    int rc = sqlite3_prepare_v2(db, check_sql, -1, &check_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "delete member check preparation");
        return -1;
    }

    sqlite3_bind_int(check_stmt, 1, id);
    rc = sqlite3_step(check_stmt);
    int active_loans = 0;
    if (rc == SQLITE_ROW) {
        active_loans = sqlite3_column_int(check_stmt, 0);
    }
    sqlite3_finalize(check_stmt);

    if (active_loans > 0) {
        printf(COLOR_RED "Error: Cannot delete member. They currently have %d borrowed books.\n" COLOR_RESET, active_loans);
        return -1;
    }

    // 2. Perform the deletion
    const char* delete_sql = "DELETE FROM members WHERE id = ?;";
    sqlite3_stmt* delete_stmt = NULL;
    rc = sqlite3_prepare_v2(db, delete_sql, -1, &delete_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "delete member preparation");
        return -1;
    }

    sqlite3_bind_int(delete_stmt, 1, id);
    rc = sqlite3_step(delete_stmt);
    int affected = sqlite3_changes(db);
    sqlite3_finalize(delete_stmt);

    if (rc == SQLITE_DONE) {
        if (affected > 0) {
            printf(COLOR_GREEN "Success: Member record deleted successfully.\n" COLOR_RESET);
            return 0;
        } else {
            printf(COLOR_RED "Error: Member with ID %d not found.\n" COLOR_RESET, id);
            return -1;
        }
    } else if (rc == SQLITE_CONSTRAINT) {
        // Typically foreign key check violation if member has loan history
        printf(COLOR_RED "Error: Cannot delete member because they have loan history records.\n" 
               "To preserve data integrity, SQLite blocks deletion of this entity.\n" COLOR_RESET);
        return -1;
    } else {
        print_db_error(db, "deleting member");
        return -1;
    }
}


// --- Loan Operations ---

int borrow_book(sqlite3* db, int book_id, int member_id, int days_to_due) {
    if (days_to_due <= 0) {
        printf(COLOR_RED "Error: Borrow period must be at least 1 day.\n" COLOR_RESET);
        return -1;
    }

    int rc;
    char* err_msg = NULL;

    // Begin Transaction to guarantee database consistency
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, COLOR_RED "Error starting transaction: %s\n" COLOR_RESET, err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    // 1. Verify and retrieve availability of the book
    const char* book_sql = "SELECT available, title FROM books WHERE id = ?;";
    sqlite3_stmt* book_stmt = NULL;
    rc = sqlite3_prepare_v2(db, book_sql, -1, &book_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "borrow check book preparation");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    sqlite3_bind_int(book_stmt, 1, book_id);
    rc = sqlite3_step(book_stmt);
    if (rc != SQLITE_ROW) {
        printf(COLOR_RED "Error: Book with ID %d does not exist.\n" COLOR_RESET, book_id);
        sqlite3_finalize(book_stmt);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    int available = sqlite3_column_int(book_stmt, 0);
    const char* book_title = (const char*)sqlite3_column_text(book_stmt, 1);
    char saved_title[256] = {0};
    if (book_title) strncpy(saved_title, book_title, sizeof(saved_title) - 1);
    sqlite3_finalize(book_stmt);

    if (available <= 0) {
        printf(COLOR_RED "Error: Book '%s' is currently out of stock.\n" COLOR_RESET, saved_title);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }

    // 2. Verify existence of the member
    const char* member_sql = "SELECT name FROM members WHERE id = ?;";
    sqlite3_stmt* member_stmt = NULL;
    rc = sqlite3_prepare_v2(db, member_sql, -1, &member_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "borrow check member preparation");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    sqlite3_bind_int(member_stmt, 1, member_id);
    rc = sqlite3_step(member_stmt);
    if (rc != SQLITE_ROW) {
        printf(COLOR_RED "Error: Member with ID %d does not exist.\n" COLOR_RESET, member_id);
        sqlite3_finalize(member_stmt);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    const char* member_name = (const char*)sqlite3_column_text(member_stmt, 0);
    char saved_name[256] = {0};
    if (member_name) strncpy(saved_name, member_name, sizeof(saved_name) - 1);
    sqlite3_finalize(member_stmt);

    // 3. Insert Loan Record
    // SQLite's date modifier string e.g. "+14 days"
    char days_modifier[32];
    snprintf(days_modifier, sizeof(days_modifier), "+%d days", days_to_due);

    const char* loan_sql = "INSERT INTO loans (book_id, member_id, borrow_date, due_date, return_date) "
                           "VALUES (?, ?, DATE('now'), DATE('now', ?), NULL);";
    sqlite3_stmt* loan_stmt = NULL;
    rc = sqlite3_prepare_v2(db, loan_sql, -1, &loan_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "borrow insert loan preparation");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    sqlite3_bind_int(loan_stmt, 1, book_id);
    sqlite3_bind_int(loan_stmt, 2, member_id);
    sqlite3_bind_text(loan_stmt, 3, days_modifier, -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(loan_stmt);
    sqlite3_finalize(loan_stmt);
    if (rc != SQLITE_DONE) {
        print_db_error(db, "inserting loan record");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }

    // 4. Decrement available book count
    const char* update_book_sql = "UPDATE books SET available = available - 1 WHERE id = ?;";
    sqlite3_stmt* update_stmt = NULL;
    rc = sqlite3_prepare_v2(db, update_book_sql, -1, &update_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "borrow decrement available count preparation");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    sqlite3_bind_int(update_stmt, 1, book_id);
    rc = sqlite3_step(update_stmt);
    sqlite3_finalize(update_stmt);
    if (rc != SQLITE_DONE) {
        print_db_error(db, "updating book availability");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }

    // Commit transaction
    rc = sqlite3_exec(db, "COMMIT;", 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, COLOR_RED "Error committing transaction: %s\n" COLOR_RESET, err_msg);
        sqlite3_free(err_msg);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }

    printf(COLOR_GREEN "Success: '%s' successfully checked out to '%s'.\n" COLOR_RESET, saved_title, saved_name);
    return 0;
}

int return_book(sqlite3* db, int loan_id) {
    int rc;
    char* err_msg = NULL;

    // Begin Transaction
    rc = sqlite3_exec(db, "BEGIN TRANSACTION;", 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, COLOR_RED "Error starting transaction: %s\n" COLOR_RESET, err_msg);
        sqlite3_free(err_msg);
        return -1;
    }

    // 1. Check if the loan exists and is active (not returned yet)
    const char* loan_sql = "SELECT book_id, return_date FROM loans WHERE id = ?;";
    sqlite3_stmt* loan_stmt = NULL;
    rc = sqlite3_prepare_v2(db, loan_sql, -1, &loan_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "return check loan preparation");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    sqlite3_bind_int(loan_stmt, 1, loan_id);
    rc = sqlite3_step(loan_stmt);
    if (rc != SQLITE_ROW) {
        printf(COLOR_RED "Error: Loan record with ID %d not found.\n" COLOR_RESET, loan_id);
        sqlite3_finalize(loan_stmt);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    int book_id = sqlite3_column_int(loan_stmt, 0);
    const char* return_date = (const char*)sqlite3_column_text(loan_stmt, 1);
    
    if (return_date != NULL && strlen(return_date) > 0) {
        printf(COLOR_YELLOW "Info: Book has already been returned for this loan on %s.\n" COLOR_RESET, return_date);
        sqlite3_finalize(loan_stmt);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    sqlite3_finalize(loan_stmt);

    // 2. Mark loan as returned
    const char* update_loan_sql = "UPDATE loans SET return_date = DATE('now') WHERE id = ?;";
    sqlite3_stmt* update_loan_stmt = NULL;
    rc = sqlite3_prepare_v2(db, update_loan_sql, -1, &update_loan_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "return update loan preparation");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    sqlite3_bind_int(update_loan_stmt, 1, loan_id);
    rc = sqlite3_step(update_loan_stmt);
    sqlite3_finalize(update_loan_stmt);
    if (rc != SQLITE_DONE) {
        print_db_error(db, "updating loan record");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }

    // 3. Increment book availability
    const char* update_book_sql = "UPDATE books SET available = available + 1 WHERE id = ?;";
    sqlite3_stmt* update_book_stmt = NULL;
    rc = sqlite3_prepare_v2(db, update_book_sql, -1, &update_book_stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "return increment availability preparation");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }
    sqlite3_bind_int(update_book_stmt, 1, book_id);
    rc = sqlite3_step(update_book_stmt);
    sqlite3_finalize(update_book_stmt);
    if (rc != SQLITE_DONE) {
        print_db_error(db, "updating book availability");
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }

    // Commit Transaction
    rc = sqlite3_exec(db, "COMMIT;", 0, 0, &err_msg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, COLOR_RED "Error committing transaction: %s\n" COLOR_RESET, err_msg);
        sqlite3_free(err_msg);
        sqlite3_exec(db, "ROLLBACK;", 0, 0, NULL);
        return -1;
    }

    printf(COLOR_GREEN "Success: Book has been returned successfully.\n" COLOR_RESET);
    return 0;
}

static void print_loan_table_header() {
    printf(COLOR_CYAN);
    printf("┌──────────┬──────────────────────────────┬────────────────────────┬────────────┬────────────┬────────────┐\n");
    printf("│ Loan ID  │ Book Title                   │ Member Name            │ Borrowed   │ Due Date   │ Status     │\n");
    printf("├──────────┼──────────────────────────────┼────────────────────────┼────────────┼────────────┼────────────┤\n");
    printf(COLOR_RESET);
}

static void print_loan_table_row(int id, const char* title, const char* member, const char* borrow_date, const char* due_date, const char* return_date) {
    char id_str[16];
    snprintf(id_str, sizeof(id_str), "%d", id);

    printf("│ ");
    print_cell(id_str, 8, 0);
    printf(" │ ");
    print_cell(title, 28, 1);
    printf(" │ ");
    print_cell(member, 22, 1);
    printf(" │ ");
    print_cell(borrow_date, 10, 1);
    printf(" │ ");
    print_cell(due_date, 10, 1);
    printf(" │ ");
    if (return_date == NULL || strlen(return_date) == 0) {
        printf(COLOR_YELLOW "Active      " COLOR_RESET);
    } else {
        printf(COLOR_GREEN);
        print_cell(return_date, 10, 1);
        printf(COLOR_RESET);
    }
    printf(" │\n");
}

static void print_loan_table_footer() {
    printf(COLOR_CYAN);
    printf("└──────────┴──────────────────────────────┴────────────────────────┴────────────┴────────────┴────────────┘\n");
    printf(COLOR_RESET);
}

int list_active_loans(sqlite3* db) {
    const char* sql = "SELECT l.id, b.title, m.name, l.borrow_date, l.due_date, l.return_date "
                      "FROM loans l "
                      "JOIN books b ON l.book_id = b.id "
                      "JOIN members m ON l.member_id = m.id "
                      "WHERE l.return_date IS NULL "
                      "ORDER BY l.id ASC;";
    sqlite3_stmt* stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "list active loans preparation");
        return -1;
    }

    print_loan_table_header();
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* title = (const char*)sqlite3_column_text(stmt, 1);
        const char* member_name = (const char*)sqlite3_column_text(stmt, 2);
        const char* borrow_date = (const char*)sqlite3_column_text(stmt, 3);
        const char* due_date = (const char*)sqlite3_column_text(stmt, 4);
        const char* return_date = (const char*)sqlite3_column_text(stmt, 5);

        print_loan_table_row(id, title, member_name, borrow_date, due_date, return_date);
        count++;
    }
    print_loan_table_footer();

    sqlite3_finalize(stmt);
    printf("Total active loans: %d\n", count);
    return 0;
}

int list_loan_history(sqlite3* db) {
    const char* sql = "SELECT l.id, b.title, m.name, l.borrow_date, l.due_date, l.return_date "
                      "FROM loans l "
                      "JOIN books b ON l.book_id = b.id "
                      "JOIN members m ON l.member_id = m.id "
                      "ORDER BY l.id DESC;";
    sqlite3_stmt* stmt = NULL;

    int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        print_db_error(db, "list loan history preparation");
        return -1;
    }

    print_loan_table_header();
    int count = 0;
    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        const char* title = (const char*)sqlite3_column_text(stmt, 1);
        const char* member_name = (const char*)sqlite3_column_text(stmt, 2);
        const char* borrow_date = (const char*)sqlite3_column_text(stmt, 3);
        const char* due_date = (const char*)sqlite3_column_text(stmt, 4);
        const char* return_date = (const char*)sqlite3_column_text(stmt, 5);

        print_loan_table_row(id, title, member_name, borrow_date, due_date, return_date);
        count++;
    }
    print_loan_table_footer();

    sqlite3_finalize(stmt);
    printf("Total historical loan records: %d\n", count);
    return 0;
}
