CFLAGS = -Wall -g

CC = gcc
CFLAGS = -Wall -g

all: order

order: orders.c books.c books.h node.c node.h queue.c queue.h
	$(CC) $(CFLAGS) -lpthread -o order *.c

books: books.c node.c queue.c
	$(CC) $(CFLAGS) -c books.c node.c queue.c

node: orders.c books.c node.c queue.c
	$(CC) $(CFLAGS) -o order orders.c books.c node.c queue.c

clean:
	rm -f *.o
	rm -f order
