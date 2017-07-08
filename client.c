#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "libs/helper.h"

int listenFileDescriptor;

void closeFromUser(int);

int main (int argc, const char * argv[]) {

    // Variables relatives aux informations du serveur
    int maxSocket;
    char readBuffer[100], writeBuffer[100], word[100], state = 0;
    fd_set readFS;

    // Informations du serveur
    listenFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);

    // Connexion au serveur
    connectServer(listenFileDescriptor, "127.0.0.1", PORT);

    // Permet la fermeture propre si la partie n'est pas terminée
    signal(SIGINT, closeFromUser);

    maxSocket = listenFileDescriptor + 1;

    while(1) {
        FD_ZERO(&readFS);
        FD_SET(listenFileDescriptor, &readFS);

        select(maxSocket, &readFS, NULL, NULL, NULL);

        if (FD_ISSET(listenFileDescriptor, &readFS)) {
            // Réponse du serveur
            read(listenFileDescriptor, readBuffer, sizeof(readBuffer));

            // Récupération de l'état de jeu
            state = readBuffer[0];

            // Récupération du mot
            strncpy(word, readBuffer+1, strlen(readBuffer)-1);

            // Affichage de l'état
            if (state == '0') {
                printf("Wrong answer!\n");
            } else if (state == '1') {
                printf("Good guess!\n");
            } else if (state == '2') {
                printf("You lost! The word was (TO BE COMPLETED)\n");
            } else if (state == '3') {
                printf("You won, good job!\n");
            } else if (state == '4') {
                printf("Starting game... good luck!\n");
            }

            // Affichage des pointillés
            printf("%s\n", word);

            if (state == '2' || state == '3') {
                close(listenFileDescriptor);
                return 0;
            } else {
                printf("Type something: ");
                fgets(writeBuffer, sizeof(writeBuffer), stdin);
                write(listenFileDescriptor, writeBuffer, sizeof(writeBuffer));
            }
        }
    }
}

void closeFromUser(int signal) {
    close(listenFileDescriptor);
    exit(0);
}