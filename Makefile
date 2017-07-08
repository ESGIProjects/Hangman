all: client server

client: client.o hangman.o helper.o
	gcc client.o hangman.o helper.o -o client

client.o: client.c libs/hangman.h libs/hangman.c libs/helper.h libs/helper.c
	gcc -c client.c -o client.o

server: server.o hangman.o helper.o
	gcc server.o hangman.o helper.o -o server

server.o: server.c libs/hangman.h libs/hangman.c libs/helper.h libs/helper.c
	gcc -lpthread -c server.c -o server.o

hangman.o: libs/hangman.h libs/hangman.c
	gcc -c libs/hangman.c -o hangman.o

helper.o: libs/helper.h libs/helper.c
	gcc -c libs/helper.c -o helper.o

clean:
	rm -rf *.o
