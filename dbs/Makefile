CC = clang
CFLAGS = -Wall -Wextra -O2 -std=c99 -I/opt/homebrew/opt/sqlite/include -Isrc
LDFLAGS = -L/opt/homebrew/opt/sqlite/lib -lsqlite3

TARGET = library_mgr
SRC_DIR = src
OBJ_DIR = obj

SRCS = $(SRC_DIR)/db.c $(SRC_DIR)/library.c $(SRC_DIR)/main.c
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)

run: all
	./$(TARGET)
