all: client server client2

client:
	gcc client.c -o client -lpthread 

server:
	gcc server.c -o server -lpthread 

client2:
	gcc client2.c -o client2 -lpthread 

clean:
	rm server client client2
