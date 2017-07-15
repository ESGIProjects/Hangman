#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include "libs/helper.h"

#define RED "\x1B[31m"
#define BLUE "\x1B[34m"
#define GREEN "\x1B[32m"
#define RESET "\x1B[0m"

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
            ssize_t readValue = read(listenFileDescriptor, readBuffer, sizeof(readBuffer));

            if (readValue == 0) {
                printf(RED "\n\nSorry, the server isn't responding anymore. You can retry later!\n\n" RESET);
                close(listenFileDescriptor);
                exit(0);
            }

            // Récupération de l'état de jeu
            state = readBuffer[0];

            // Récupération du mot
            strncpy(word, readBuffer+1, strlen(readBuffer)-1);

            // Affichage de l'état
            if (state == '0') {
                printf(RED "\nWrong answer!\n" RESET);
            } else if (state == '1') {
                printf(GREEN "\nGood guess!\n" RESET);
            } else if (state == '2') {
                printf(RED "\nYou lost! The word was \"%s\"\nWanna try again? Relauch the game!\n\n" RESET, word);
            } else if (state == '3') {
                printf(GREEN "\nYou won, good job!\n" RESET);
            } else if (state == '4') {
                printf(BLUE "\nStarting game... good luck!\n" RESET);
            }

            // Affichage des pointillés si le jeu n'est pas perdu
            if (state != '2')
                printf("The word is: %s\n\n", word);

            if (state == '2' || state == '3') {
                close(listenFileDescriptor);
                return 0;
            } else {
                printf(BLUE "Guess a new letter!\nYou must type a lowercased letter, or else it will be wrong.\n");
                printf("Type something: " RESET);
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