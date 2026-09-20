CC = clang
CFLAGS = -Wall -Wextra -g

all: linked_list.out

linked_list.out: linked_list.o linked_list_fs.o
	$(CC) $(CFLAGS) -o linked_list.out linked_list.o linked_list_fs.o

linked_list.o: linked_list.c linked_list_fs.h
	$(CC) $(CFLAGS) -c linked_list.c

linked_list_fs.o: linked_list_fs.c linked_list_fs.h
	$(CC) $(CFLAGS) -c linked_list_fs.c

clean:
	rm -f *.o linked_list.out