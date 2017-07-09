#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sys/time.h>
#include <pthread.h>

#include "libs/hangman.h"
#include "libs/helper.h"

#define MAX_SOCKETS_PER_THREAD 5
#define MAX_THREADS 10

int maxSocket, threadsCount = 0;
threadArgs * threads[MAX_THREADS];

void * gameThread(void * args) {
    threadArgs * thread = threads[(int)args];

    char readBuffer[100], writeBuffer[100];

    int i;
    fd_set readFS;

    while(1) {

        pthread_mutex_lock(&(*thread).mutex);
        if ((*thread).socketsCount <= 0) {
            printf("start wait\n");
            pthread_cond_wait(&(*thread).condition, &(*thread).mutex);
            printf("stop wait\n");
        }
        pthread_mutex_unlock(&(*thread).mutex);

        // Mise en place de l'écoute concurrente
        FD_ZERO(&readFS);
        for (i = 0; i < (*thread).socketsCount; i++) {
            FD_SET((*thread).sockets[i], &readFS);
        }

        struct timeval tv = {1, 0};
        select(maxSocket, &readFS, NULL, NULL, &tv);

        for (i = 0; i < (*thread).socketsCount; i++) {
            if (FD_ISSET((*thread).sockets[i], &readFS)) {

                // Réception
                ssize_t readValue = read((*thread).sockets[i], readBuffer, sizeof(readBuffer));

                if (readValue == 0) {
                    printf("Socket closed by client\n");
                    close((*thread).sockets[i]);
                    removeInteger((*thread).sockets, i, (*thread).socketsCount);
                    removeInteger((*thread).availableTries, i, (*thread).socketsCount);
                    removeString((*thread).selectedWords, i, (*thread).socketsCount);
                    removeString((*thread).dashedWords, i, (*thread).socketsCount);

                    (*thread).socketsCount--;

                    continue;
                }

                // On conserve seulement le premier caractère
                char letterTyped = readBuffer[0];
                int replacedLetters = checkAnswer(letterTyped, (*thread).selectedWords[i], (*thread).dashedWords[i]);

                // 0 False answer
                // 1 Good answer
                // 2 Game Over (lost)
                // 3 Game Over (won)
                // 4 Start Game

                if (replacedLetters == 0) {
                    // Wrong guess
                    (*thread).availableTries[i]--;
                    if ((*thread).availableTries[i] > 0) {
                        // Wrong guess but not over
                        strcpy(writeBuffer, "0");
                    } else {
                        // Game over
                        strcpy(writeBuffer, "2");
                    }
                } else {
                    // Good guess
                    if (strcmp((*thread).dashedWords[i], (*thread).selectedWords[i]) == 0) {
                        // Player won
                        strcpy(writeBuffer, "3");
                    } else {
                        // Good guess but not over
                        strcpy(writeBuffer, "1");
                    }
                }

                // Ajout du mot au buffer
                strcat(writeBuffer, (*thread).dashedWords[i]);

                // Réponse au client
                printf("%s\n", writeBuffer);
                write((*thread).sockets[i], writeBuffer, sizeof(writeBuffer));

                // On vérifie si le jeu est terminé
                if (writeBuffer[0] == '2' || writeBuffer[0] == '3') {
                    printf("Socket closed (game over)\n");
                    close((*thread).sockets[i]);

                    removeInteger((*thread).sockets, i, (*thread).socketsCount);
                    removeInteger((*thread).availableTries, i, (*thread).socketsCount);
                    removeString((*thread).selectedWords, i, (*thread).socketsCount);
                    removeString((*thread).dashedWords, i, (*thread).socketsCount);

                    (*thread).socketsCount--;
                }
            }
        }
    }
}

int main(int argc, const char * argv[]) {

    int i = 0;

    // Variables relatives à la récupération des mots
    int wordsTotal = 0, wordsFileDescriptor;
    char ** wordsList;

    // Variables relatives aux informations du serveur
    char writeBuffer[100];
    int listenFileDescriptor, connectFileDescriptor;
    struct sockaddr_in serverInfo;

    // Variables relatives à la concurrence du serveur
    int socketsCount = 0;

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
    serverInfo.sin_port = htons(PORT);

    bind(listenFileDescriptor, (struct sockaddr*) &serverInfo, sizeof(serverInfo));

    // Lancement de l'écoute

    if (!listen(listenFileDescriptor, 5)) {
        printf("Listening\n");
    } else {
        printf("Error while listening\n");
        return EXIT_FAILURE;
    }

    // Écoute des nouvelles connexions

    maxSocket = listenFileDescriptor + 1;

    while ((connectFileDescriptor = acceptConnection(listenFileDescriptor))) {

        // 1. Sélection d'un thread
        threadArgs * thread = NULL;
        int isCreating = 0;

        if (socketsCount % MAX_SOCKETS_PER_THREAD == 0) {
            // Création
            printf("- Creating thread no. %d -\n", threadsCount);
            threads[threadsCount] = malloc(sizeof(threadArgs));

            thread = threads[threadsCount];
            thread->sockets = (int*) malloc(MAX_SOCKETS_PER_THREAD * sizeof(int));
            thread->socketsCount = 0;
            pthread_mutex_init(&thread->mutex, NULL);
            pthread_cond_init(&thread->condition, NULL);
            isCreating = 1;
        } else {
            // Assignement du dernier
            printf("- Adding socket to thread no. %d -\n", threadsCount-1);
            thread = threads[threadsCount-1];
            isCreating = 0;
        }

        socketsCount++;

//        // On récupère un thread existant si l'on peut
//        for (i = 0; i < threadsCount; i++) {
//            if (threads[i]->socketsCount < MAX_SOCKETS_PER_THREAD) {
//                printf("- Adding socket to thread no. %d -\n", i);
//                thread = threads[i];
//                isCreating = 0;
//                break;
//            }
//        }
//
//        // Sinon, création d'un nouveau thread
//        if (thread == NULL) {
//            printf("- Creating thread no. %d -\n", threadsCount);
//            threads[threadsCount] = malloc(sizeof(threadArgs));
//
//            thread = threads[threadsCount];
//            thread->sockets = (int*) malloc(MAX_SOCKETS_PER_THREAD * sizeof(int));
//            thread->socketsCount = 0;
//            pthread_mutex_init(&thread->mutex, NULL);
//            pthread_cond_init(&thread->condition, NULL);
//            isCreating = 1;
//        }

        maxSocket = connectFileDescriptor + 1;

        thread->sockets[thread->socketsCount] = connectFileDescriptor;

        // Mise en place de la partie
        int random = randomNumber(0, wordsTotal);
        thread->selectedWords[thread->socketsCount] = wordsList[random];
        printf("Selected word: %s\n", wordsList[random]);

        thread->dashedWords[thread->socketsCount] = getDashedWord(thread->selectedWords[thread->socketsCount]);
        thread->availableTries[thread->socketsCount] = 10;

        // On envoie le mot caché au client
        strcpy(writeBuffer, "4");
        strcat(writeBuffer, thread->dashedWords[thread->socketsCount]);
        write(connectFileDescriptor, writeBuffer, sizeof(writeBuffer));

        // On incrémente enfin le nombre de connexions conservées
        thread->socketsCount++;

        pthread_mutex_lock(&thread->mutex);
        if (thread->socketsCount == 1 && isCreating == 0)
            pthread_cond_signal(&thread->condition);
        pthread_mutex_unlock(&thread->mutex);

        if (isCreating == 1) {
            pthread_create(&(thread->thread), NULL, gameThread, (void *) threadsCount++);
        }

        for (i = 0; i < threadsCount; i++) {
            printf("- Number of sockets in thread no. %d : %d -\n", i, threads[i]->socketsCount);
        }
    }

    close(listenFileDescriptor); // à passer en signal via ctrl-c

    return 0;

}