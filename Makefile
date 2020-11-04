all: server.o

server.o: server.c
	gcc server.c -o server
clean:
	rm -f server

