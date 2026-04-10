CC = gcc
CFLAGS = -Wall -Wextra -O2

all: server client test

server: server.c common.h
	$(CC) $(CFLAGS) server.c -o server

client: client.c common.h
	$(CC) $(CFLAGS) client.c -o client

test: test.c
	$(CC) $(CFLAGS) test.c -o test

clean:
	rm -f server client test received_exec exec_bin output.txt compile_error.txt

run-server1:
	./server 8080

run-server2:
	./server 8081

run-client:
	./client

.PHONY: all clean run-server1 run-server2 run-client
