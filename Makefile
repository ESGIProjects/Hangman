all: client.out server.out

client: client.o hangman.o helper.o
	gcc client.o hangman.o helper.o -o client.out

client.o: client.c libs/hangman.c libs/helper.c
	gcc -c client.c -o client.o

server: server.o hangman.o helper.o
	gcc server.o hangman.o helper.o -o server.out

server.o: server.c libs/hangman.c libs/helper.c
	gcc -c server.c -o server.o

hangman.o: libs/hangman.c
	gcc -c libs/hangman.c -o hangman.o

helper.o: libs/helper.c
	gcc -c libs/helper.c -o helper.o

clean:
	rm -rf *.o
