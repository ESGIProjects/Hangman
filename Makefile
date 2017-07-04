all: client.out server.out

client: client.o hangman.o helper.o
	gcc client.o hangman.o helper.o -o client.out

client.o: client.c hangman.c helper.c
	gcc -c client.c -o client.o

server: server.o hangman.o helper.o
	gcc server.o hangman.o helper.o -o server.out

server.o: server.c hangman.c helper.c
	gcc -c server.c -o server.o

hangman.o: hangman.c
	gcc -c hangman.c -o hangman.o

helper.o: helper.c
	gcc -c helper.c -o helper.o

clean:
	rm -rf *.o
	rm -rf *.out
