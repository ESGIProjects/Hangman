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

    int i = 0;

    // Variables relatives à la récupération des mots
    int wordsTotal = 0, wordsFileDescriptor;
    char ** wordsList;

    // Variables relatives aux informations du serveur
    char readBuffer[100], writeBuffer[100];
    int listenFileDescriptor, connectFileDescriptor;
    struct sockaddr_in serverInfo;

    // Variables relatives à la concurrence du serveur
    int maxSocket, * sockets, socketsCount;
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
    serverInfo.sin_port = htons(7779);

    bind(listenFileDescriptor, (struct sockaddr*) &serverInfo, sizeof(serverInfo));

    // Lancement de l'écoute

    if (!listen(listenFileDescriptor, 5)) {
        printf("Listening\n");
    } else {
        printf("Error\n");
    }

    // Concurrence

    maxSocket = listenFileDescriptor + 1;
    socketsCount = 1;
    sockets = (int*) malloc(10 * sizeof(int));
    sockets[0] = listenFileDescriptor;

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
            sockets = (int *) realloc(sockets, (socketsCount+1) * sizeof(int));
            sockets[socketsCount] = connectFileDescriptor;

            // Mise en place de la partie
            int random = randomNumber(0, wordsTotal);
            selectedWords[socketsCount] = wordsList[random];
            printf("random number : %d\n", random);
            dashedWords[socketsCount] = getDashedWord(selectedWords[socketsCount]);
            availableTries[socketsCount] = 10;

            // On envoie le mot caché au client
            strcpy(writeBuffer, "4");
            strcat(writeBuffer, dashedWords[socketsCount]);
            write(connectFileDescriptor, writeBuffer, sizeof(writeBuffer));

            // On incrémente enfin le nombre de connexions conservées
            socketsCount++;
        }


        // Client connu
        for (i = 1; i < socketsCount; i++) {

            // On récupère la connexion concernée
            if (FD_ISSET(sockets[i], &readFS)) {

                // Réception
                read(sockets[i], readBuffer, sizeof(readBuffer));

                // On conserve seulement le premier caractère
                char letterTyped = readBuffer[0];
                int replacedLetters = checkAnswer(letterTyped, selectedWords[i], dashedWords[i]);

                // 0 False answer
                // 1 Good answer
                // 2 Game Over (lost)
                // 3 Game Over (won)
                // 4 Start Game

                if (replacedLetters == 0) {
                    // Wrong guess
                    if (availableTries[i] > 0) {
                        // Wrong guess but not over
                        strcpy(writeBuffer, "0");
                        availableTries[i]--;
                    } else {
                        // Game over
                        strcpy(writeBuffer, "2");
                    }
                } else {
                    // Good guess
                    if (strcmp(dashedWords[i], selectedWords[i]) == 0) {
                        // Player won
                        strcpy(writeBuffer, "3");
                    }
                    else {
                        // Good guess but not over
                        strcpy(writeBuffer, "1");
                    }
                }

                // Ajout du mot au buffer
                strcat(writeBuffer, dashedWords[i]);

                // Réponse au client
                printf("%s\n", writeBuffer);
                write(sockets[i], writeBuffer, sizeof(writeBuffer));

                // On vérifie si le jeu est terminé
                if (writeBuffer[0] == '2' || writeBuffer[0] == '3') {
                    printf("CLOSE\n");
                    close(sockets[i]);
                    availableTries[i] = 10;
                    selectedWords[i] = NULL;
                    dashedWords[i] = NULL;

                    deleteIntegerFromArray(&sockets, &socketsCount, i);
                }
            }
        }
    }

    close(listenFileDescriptor); // à passer en signal via ctrl-c

    return 0;

}