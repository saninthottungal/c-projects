#!/bin/bash

# Exit immediately if any command fails
set -e

# Make sure the project is compiled
echo "Building the project..."
make

# Define test database name
TEST_DB="test_library.db"

# Clear any previous test run
rm -f "$TEST_DB"

echo "Running interactive simulation..."

# Feed actions to library_mgr:
# 1. Add Book: "The C Programming Language" by "Brian Kernighan" (ISBN: 978-0131103627), Qty: 3
# 2. Add Member: "Alice" (Email: alice@example.com, Phone: 555-0199)
# 3. Borrow book: Book ID 1, Member ID 1, duration 10 days
# 4. List Books, List Members, List Loans to check console tables
# 5. Exit
./library_mgr "$TEST_DB" <<EOF
1
1
The C Programming Language
Brian Kernighan
978-0131103627
3

2

0
2
1
Alice
alice@example.com
555-0199

2

0
3
1
1
1
10

3

0
0
EOF

echo ""
echo "=================================================="
echo "    DIRECT DB INTEGRITY & PERSISTENCE VERIFICATION"
echo "=================================================="
echo ""

# Query books table
echo "--> Books in DB (Expect: Available 2, Total 3):"
/opt/homebrew/opt/sqlite/bin/sqlite3 "$TEST_DB" ".headers on" ".mode column" "SELECT id, title, isbn, quantity, available FROM books;"
echo ""

# Query members table
echo "--> Members in DB (Expect: Alice):"
/opt/homebrew/opt/sqlite/bin/sqlite3 "$TEST_DB" ".headers on" ".mode column" "SELECT id, name, email, join_date FROM members;"
echo ""

# Query loans table
echo "--> Active Loans in DB (Expect: 1 loan, unreturned):"
/opt/homebrew/opt/sqlite/bin/sqlite3 "$TEST_DB" ".headers on" ".mode column" "SELECT id, book_id, member_id, borrow_date, due_date, return_date FROM loans;"
echo ""

# Test SQL Constraint: Try borrowing when available count is 0
echo "--> Simulating checkout when quantity is depleted..."
# Add a second member "Bob" and borrow twice more so availability is 0. Then try borrowing a 4th time (should fail/rollback).
./library_mgr "$TEST_DB" <<EOF
2
1
Bob
bob@example.com
555-0200

0
3
1
1
2
14

1
1
2
14

1
1
2
14

0
0
EOF

echo ""
echo "--> Books in DB after extra borrows (Expect: Available 0, Total 3):"
/opt/homebrew/opt/sqlite/bin/sqlite3 "$TEST_DB" ".headers on" ".mode column" "SELECT id, title, available FROM books;"
echo ""

# Return a book
echo "--> Returning a book (Loan ID 1)..."
./library_mgr "$TEST_DB" <<EOF
3
2
1

3

0
0
EOF

echo ""
echo "--> Books in DB after return (Expect: Available 1, Total 3):"
/opt/homebrew/opt/sqlite/bin/sqlite3 "$TEST_DB" ".headers on" ".mode column" "SELECT id, title, available FROM books;"
echo ""

# Clean up test DB after test completes
rm -f "$TEST_DB"
echo "Verification tests completed successfully!"
