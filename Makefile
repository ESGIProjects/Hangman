all: client server

client: client.c hangman.o helper.o
	gcc client.c hangman.o helper.o -o client

server: server.c hangman.o helper.o
	gcc -lpthread server.c hangman.o helper.o -o server

hangman.o: libs/hangman.h libs/hangman.c
	gcc -c libs/hangman.c -o hangman.o

helper.o: libs/helper.h libs/helper.c
	gcc -c libs/helper.c -o helper.o

clean:
	rm -rf *.o
