#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"
#include "library.h"

// Terminal color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_MAGENTA "\033[1;35m"

// Safe input helper functions
static void get_input_str(const char* prompt, char* buffer, size_t max_len) {
    printf("%s", prompt);
    fflush(stdout);
    if (fgets(buffer, max_len, stdin) != NULL) {
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }
    } else {
        buffer[0] = '\0';
    }
}

static int get_input_int(const char* prompt) {
    char buf[64];
    get_input_str(prompt, buf, sizeof(buf));
    return atoi(buf);
}

static void press_enter_to_continue() {
    printf(COLOR_YELLOW "\nPress Enter to continue..." COLOR_RESET);
    fflush(stdout);
    char buf[16];
    fgets(buf, sizeof(buf), stdin);
}

static void clear_screen() {
    // ANSI clear screen code
    printf("\033[H\033[J");
}

static void print_banner(const char* title) {
    clear_screen();
    printf(COLOR_BLUE "======================================================================\n" COLOR_RESET);
    printf(COLOR_CYAN COLOR_BOLD "  %-66s\n" COLOR_RESET, title);
    printf(COLOR_BLUE "======================================================================\n" COLOR_RESET);
}

// Menu runners
static void menu_books(sqlite3* db) {
    int choice;
    do {
        print_banner("LIBRARY MANAGEMENT SYSTEM - BOOK INVENTORY");
        printf(" 1. Add New Book\n");
        printf(" 2. List All Books\n");
        printf(" 3. Search Books\n");
        printf(" 4. Update Book Details\n");
        printf(" 5. Delete Book\n");
        printf(" 0. Back to Main Menu\n\n");
        choice = get_input_int("Enter option: ");

        switch (choice) {
            case 1: {
                print_banner("ADD NEW BOOK");
                char title[256], author[256], isbn[64];
                get_input_str("Enter Title: ", title, sizeof(title));
                get_input_str("Enter Author: ", author, sizeof(author));
                get_input_str("Enter ISBN: ", isbn, sizeof(isbn));
                int quantity = get_input_int("Enter Total Quantity: ");

                if (strlen(title) == 0 || strlen(author) == 0 || strlen(isbn) == 0) {
                    printf(COLOR_RED "Error: Title, Author, and ISBN cannot be empty.\n" COLOR_RESET);
                } else {
                    add_book(db, title, author, isbn, quantity);
                }
                press_enter_to_continue();
                break;
            }
            case 2: {
                print_banner("ALL BOOKS");
                list_books(db);
                press_enter_to_continue();
                break;
            }
            case 3: {
                print_banner("SEARCH BOOKS");
                char query[256];
                get_input_str("Enter search term (Title/Author/ISBN): ", query, sizeof(query));
                if (strlen(query) == 0) {
                    printf(COLOR_RED "Error: Search term cannot be empty.\n" COLOR_RESET);
                } else {
                    search_books(db, query);
                }
                press_enter_to_continue();
                break;
            }
            case 4: {
                print_banner("UPDATE BOOK");
                int id = get_input_int("Enter Book ID to update: ");
                if (id <= 0) {
                    printf(COLOR_RED "Error: Invalid Book ID.\n" COLOR_RESET);
                    press_enter_to_continue();
                    break;
                }
                
                char title[256], author[256], isbn[64];
                get_input_str("Enter New Title: ", title, sizeof(title));
                get_input_str("Enter New Author: ", author, sizeof(author));
                get_input_str("Enter New ISBN: ", isbn, sizeof(isbn));
                int quantity = get_input_int("Enter New Total Quantity: ");

                if (strlen(title) == 0 || strlen(author) == 0 || strlen(isbn) == 0) {
                    printf(COLOR_RED "Error: Fields cannot be empty.\n" COLOR_RESET);
                } else {
                    update_book(db, id, title, author, isbn, quantity);
                }
                press_enter_to_continue();
                break;
            }
            case 5: {
                print_banner("DELETE BOOK");
                int id = get_input_int("Enter Book ID to delete: ");
                if (id <= 0) {
                    printf(COLOR_RED "Error: Invalid Book ID.\n" COLOR_RESET);
                } else {
                    delete_book(db, id);
                }
                press_enter_to_continue();
                break;
            }
            case 0:
                break;
            default:
                printf(COLOR_RED "Invalid selection. Please try again.\n" COLOR_RESET);
                press_enter_to_continue();
        }
    } while (choice != 0);
}

static void menu_members(sqlite3* db) {
    int choice;
    do {
        print_banner("LIBRARY MANAGEMENT SYSTEM - MEMBERS DIRECTORY");
        printf(" 1. Register Member\n");
        printf(" 2. List All Members\n");
        printf(" 3. Search Members\n");
        printf(" 4. Update Member Details\n");
        printf(" 5. Delete Member\n");
        printf(" 0. Back to Main Menu\n\n");
        choice = get_input_int("Enter option: ");

        switch (choice) {
            case 1: {
                print_banner("REGISTER NEW MEMBER");
                char name[256], email[256], phone[64];
                get_input_str("Enter Name: ", name, sizeof(name));
                get_input_str("Enter Email: ", email, sizeof(email));
                get_input_str("Enter Phone Number: ", phone, sizeof(phone));

                if (strlen(name) == 0 || strlen(email) == 0) {
                    printf(COLOR_RED "Error: Name and Email cannot be empty.\n" COLOR_RESET);
                } else {
                    add_member(db, name, email, phone);
                }
                press_enter_to_continue();
                break;
            }
            case 2: {
                print_banner("ALL MEMBERS");
                list_members(db);
                press_enter_to_continue();
                break;
            }
            case 3: {
                print_banner("SEARCH MEMBERS");
                char query[256];
                get_input_str("Enter search term (Name/Email): ", query, sizeof(query));
                if (strlen(query) == 0) {
                    printf(COLOR_RED "Error: Search term cannot be empty.\n" COLOR_RESET);
                } else {
                    search_members(db, query);
                }
                press_enter_to_continue();
                break;
            }
            case 4: {
                print_banner("UPDATE MEMBER");
                int id = get_input_int("Enter Member ID to update: ");
                if (id <= 0) {
                    printf(COLOR_RED "Error: Invalid Member ID.\n" COLOR_RESET);
                    press_enter_to_continue();
                    break;
                }

                char name[256], email[256], phone[64];
                get_input_str("Enter New Name: ", name, sizeof(name));
                get_input_str("Enter New Email: ", email, sizeof(email));
                get_input_str("Enter New Phone: ", phone, sizeof(phone));

                if (strlen(name) == 0 || strlen(email) == 0) {
                    printf(COLOR_RED "Error: Name and Email cannot be empty.\n" COLOR_RESET);
                } else {
                    update_member(db, id, name, email, phone);
                }
                press_enter_to_continue();
                break;
            }
            case 5: {
                print_banner("DELETE MEMBER");
                int id = get_input_int("Enter Member ID to delete: ");
                if (id <= 0) {
                    printf(COLOR_RED "Error: Invalid Member ID.\n" COLOR_RESET);
                } else {
                    delete_member(db, id);
                }
                press_enter_to_continue();
                break;
            }
            case 0:
                break;
            default:
                printf(COLOR_RED "Invalid selection. Please try again.\n" COLOR_RESET);
                press_enter_to_continue();
        }
    } while (choice != 0);
}

static void menu_loans(sqlite3* db) {
    int choice;
    do {
        print_banner("LIBRARY MANAGEMENT SYSTEM - BORROW & RETURN");
        printf(" 1. Check Out Book (Borrow)\n");
        printf(" 2. Check In Book (Return)\n");
        printf(" 3. List Active Loans\n");
        printf(" 4. List Loan History\n");
        printf(" 0. Back to Main Menu\n\n");
        choice = get_input_int("Enter option: ");

        switch (choice) {
            case 1: {
                print_banner("CHECK OUT BOOK");
                int book_id = get_input_int("Enter Book ID: ");
                int member_id = get_input_int("Enter Member ID: ");
                int days = get_input_int("Enter Borrow Duration (Days, default 14): ");
                if (days <= 0) days = 14;

                if (book_id <= 0 || member_id <= 0) {
                    printf(COLOR_RED "Error: Book ID and Member ID must be valid positive integers.\n" COLOR_RESET);
                } else {
                    borrow_book(db, book_id, member_id, days);
                }
                press_enter_to_continue();
                break;
            }
            case 2: {
                print_banner("CHECK IN BOOK");
                int loan_id = get_input_int("Enter Loan ID to return: ");
                if (loan_id <= 0) {
                    printf(COLOR_RED "Error: Invalid Loan ID.\n" COLOR_RESET);
                } else {
                    return_book(db, loan_id);
                }
                press_enter_to_continue();
                break;
            }
            case 3: {
                print_banner("ACTIVE LOANS");
                list_active_loans(db);
                press_enter_to_continue();
                break;
            }
            case 4: {
                print_banner("LOAN HISTORY");
                list_loan_history(db);
                press_enter_to_continue();
                break;
            }
            case 0:
                break;
            default:
                printf(COLOR_RED "Invalid selection. Please try again.\n" COLOR_RESET);
                press_enter_to_continue();
        }
    } while (choice != 0);
}

int main(int argc, char* argv[]) {
    const char* db_file = "library.db";
    if (argc > 1) {
        db_file = argv[1];
    }

    // Connect to database
    sqlite3* db = db_connect(db_file);
    if (!db) {
        fprintf(stderr, COLOR_RED "Error: Failed to connect to database '%s'. Exiting...\n" COLOR_RESET, db_file);
        return 1;
    }

    // Initialize Schema
    int rc = db_initialize_schema(db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, COLOR_RED "Error: Failed to initialize schema. Exiting...\n" COLOR_RESET);
        db_close(db);
        return 1;
    }

    int choice;
    do {
        print_banner("LIBRARY MANAGEMENT SYSTEM (SQLITE)");
        printf(" 1. Manage Books\n");
        printf(" 2. Manage Members\n");
        printf(" 3. Borrow / Return Books\n");
        printf(" 0. Exit System\n\n");
        choice = get_input_int("Enter option: ");

        switch (choice) {
            case 1:
                menu_books(db);
                break;
            case 2:
                menu_members(db);
                break;
            case 3:
                menu_loans(db);
                break;
            case 0:
                print_banner("SHUTTING DOWN");
                printf(COLOR_GREEN "Goodbye! Thank you for using the library management system.\n\n" COLOR_RESET);
                break;
            default:
                printf(COLOR_RED "Invalid selection. Please try again.\n" COLOR_RESET);
                press_enter_to_continue();
        }
    } while (choice != 0);

    db_close(db);
    return 0;
}
