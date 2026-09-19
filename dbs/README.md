# Library Management System in C with SQLite

A premium, interactive, persistent library management system written in C and powered by SQLite.

## Features
- **Persistent Storage**: Utilizes an SQLite database (`library.db`) to persist records of books, members, and loans.
- **Transactional Safety**: Uses SQLite transactions (`BEGIN TRANSACTION`, `COMMIT`, `ROLLBACK`) for borrowing and returning books to ensure data consistency.
- **Relational Integrity**: Uses SQLite foreign key constraints to prevent deleting books/members with active borrows.
- **SQL Injection Prevention**: Employs prepared statements (`sqlite3_prepare_v2`, `sqlite3_bind_*`) for all user input.
- **Beautiful CLI Interface**: Designed with a clean, colorized ANSI terminal menu structure and auto-truncating, neatly aligned tabular displays.
- **Inventory Control**: Automatically handles book copy availability counters and blocks borrows if a book is out of stock.

---

## Directory Structure
- `src/db.h` & `src/db.c`: Database connections, schema initialization, and execution helpers.
- `src/library.h` & `src/library.c`: Struct definitions, database queries (prepared statements), business validation, and table renderers.
- `src/main.c`: Hierarchical menus, input parsers, and controllers.
- `Makefile`: Build configurations for compilation and linking.
- `verify.sh`: Scripted simulation test suite.

---

## Build and Run

### Prerequisites
- macOS with `clang` and standard development tools (Command Line Tools/Xcode).
- SQLite (installed by default on macOS, or available via Homebrew).

### Compiling
To compile the system:
```bash
make
```
This produces the executable `./library_mgr`.

### Running
To run the system with the default database file (`library.db`):
```bash
./library_mgr
```

To run with a custom database file name:
```bash
./library_mgr my_custom_library.db
```

### Running Automated Verification Tests
You can run the automated validation simulation:
```bash
./verify.sh
```

---

## Menu Layout
1. **Manage Books**
   - Add New Book (Title, Author, ISBN, Quantity)
   - List All Books (Shows ID, Title, Author, ISBN, Quantity, Available copies)
   - Search Books (Wildcard search across Title, Author, and ISBN)
   - Update Book Details (Increases/decreases total inventory safely)
   - Delete Book (Checks if actively borrowed first)
2. **Manage Members**
   - Register Member (Name, Email, Phone number, automatic Join Date)
   - List All Members
   - Search Members (Wildcard search on Name or Email)
   - Update Member Details
   - Delete Member
3. **Borrow / Return Books**
   - Check Out Book (Creates loan record, decrements book availability)
   - Check In Book (Marks loan returned, increments availability)
   - List Active Loans
   - List Loan History
