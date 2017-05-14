#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/time.h>
#include "hangman.h"
#include "helper.h"

int main(int argc, const char * argv[]) {

    int i = 0, j = 0;

    // Variables relatives à la récupération des mots
    int wordsTotal = 0, wordsFileDescriptor;
    char ** wordsList;

    // Variables relatives aux informations du serveur
    char readBuffer[100], writeBuffer[100];
    int listenFileDescriptor, connectFileDescriptor;
    struct sockaddr_in serverInfo;

    // Variables relatives à la concurrence du serveur
    int maxSocket, sockets[10], socketsCount;
    fd_set readFS;

    // Variables relatives au jeu
    char * selectedWords[10], * dashedWords[10];
    int availableTries[10];


    // Lancement de l'aléatoire
    srand((unsigned int) time(NULL));


    // Récupération des mots
    wordsFileDescriptor = openFile("words.txt");
    wordsList = readFile(wordsFileDescriptor, &wordsTotal);
    close(wordsFileDescriptor);

    // Création du serveur
    listenFileDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&serverInfo, sizeof(serverInfo));

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = htons(INADDR_ANY);
    serverInfo.sin_port = htons(7777);

    bind(listenFileDescriptor, (struct sockaddr*) &serverInfo, sizeof(serverInfo));

    // Lancement de l'écoute

    if (!listen(listenFileDescriptor, 5)) {
        printf("Listening\n");
    } else {
        printf("Error\n");
    }

    // Concurrence

    maxSocket = listenFileDescriptor + 1;
    sockets[0] = listenFileDescriptor;
    socketsCount = 1;

    while (1) {

        // Mise en place de l'écoute concurrente
        FD_ZERO(&readFS);
        for (i = 0; i < socketsCount; i++) {
            FD_SET(sockets[i], &readFS);
        }

        select(maxSocket, &readFS, NULL, NULL, NULL);


        // Nouvelle connexion
        if (FD_ISSET(sockets[0], &readFS)) {
            // Connexion acceptée et ajoutée aux autres
            connectFileDescriptor = acceptConnection(listenFileDescriptor);
            maxSocket = connectFileDescriptor + 1;
            sockets[socketsCount] = connectFileDescriptor;

            // Mise en place de la partie
            selectedWords[socketsCount] = wordsList[randomNumber(0, wordsTotal)];
            dashedWords[socketsCount] = getDashedWord(selectedWords[socketsCount]);
            availableTries[socketsCount] = 10;

            // On envoie le mot caché au client
            strcpy(writeBuffer, dashedWords[socketsCount]);
            write(connectFileDescriptor, writeBuffer, sizeof(writeBuffer));

            // On incrémente enfin le nombre de connexions conservées
            socketsCount++;
        }


        // Client connu
        for (i = 1; i < socketsCount; i++) {

            // On récupère la connexion concernée
            if (FD_ISSET(sockets[i], &readFS)) {

                // Réception
                recv(sockets[i], readBuffer, 100, 0);

                // On conserve seulement le premier caractère
                char letterTyped = readBuffer[0];
                int replacedLetters = checkAnswer(letterTyped, selectedWords[i], dashedWords[i]);

                // Réponse au client
                strcpy(writeBuffer, dashedWords[i]);
                write(sockets[i], writeBuffer, sizeof(writeBuffer));
            }
        }
    }

    close(listenFileDescriptor); // à passer en signal via ctrl-c

    return 0;

}