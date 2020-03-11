all: client server

client:
	gcc client.c -o client -lpthread -I.

server:
	gcc server.c -o server -lpthread -I.

clean:
	rm server client
