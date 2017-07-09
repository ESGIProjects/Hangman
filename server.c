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

int maxSocket;
threadArgs * threads[MAX_THREADS];

void * gameThread(void * args) {
    threadArgs * thread = threads[(int)args];

//    printf("Thread address (in thread) : %p\n", thread);
//    printf("SocketsCount value (in thread) : %d\n", (*thread)->socketsCount);

    char readBuffer[100], writeBuffer[100];

    int i;
    fd_set readFS;

    while(1) {

        // test avec mutex/cond
        //printf("SOCKETS COUNT FROM WHILE START : %d\n", (*thread)->socketsCount);

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

        //int maxSocket = (*thread)->sockets[(*thread)->socketsCount-1] + 1;

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
                    if ((*thread).availableTries[i] > 0) {
                        // Wrong guess but not over
                        strcpy(writeBuffer, "0");
                        (*thread).availableTries[i]--;
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

    //threadArgs ** threads;
    //threads = (threadArgs **) malloc(MAX_THREADS * sizeof(threadArgs *));
    int threadsCount = 0;

    // Variables relatives à la récupération des mots
    int wordsTotal = 0, wordsFileDescriptor;
    char ** wordsList;

    // Variables relatives aux informations du serveur
    char readBuffer[100], writeBuffer[100];
    int listenFileDescriptor, connectFileDescriptor;
    struct sockaddr_in serverInfo;

    // Variables relatives à la concurrence du serveur
//    int maxSocket, * sockets, socketsCount;
    int * sockets, socketsCount = 0;
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
    serverInfo.sin_port = htons(PORT);

    bind(listenFileDescriptor, (struct sockaddr*) &serverInfo, sizeof(serverInfo));

    // Lancement de l'écoute

    if (!listen(listenFileDescriptor, 5)) {
        printf("Listening\n");
    } else {
        printf("Error\n");
    }

    // Concurrence

    maxSocket = listenFileDescriptor + 1;
//    socketsCount = 1;
//    sockets = (int*) malloc(10 * sizeof(int));
//    sockets[0] = listenFileDescriptor;

    threadArgs * thr1 = NULL;
    threadArgs * thr2 = NULL;

    while ((connectFileDescriptor = acceptConnection(listenFileDescriptor))) {

        // Suppression des threads terminés
//        for (i = 0; i < threadsCount; i++) {
//            if (threads[i].socketsCount == 0) {
//                pthread_cancel(threads[i].thread);
//                removeThreads(threads, i, threadsCount--);
//            }
//        }

        // Mise en place de l'écoute concurrente
//        FD_ZERO(&readFS);
//        for (i = 0; i < socketsCount; i++) {
//            FD_SET(listenFileDescriptor, &readFS);
//        }
//
//        select(maxSocket, &readFS, NULL, NULL, NULL);

        // Nouvelle connexion
//        if (FD_ISSET(listenFileDescriptor, &readFS)) {
            // Connexion acceptée et ajoutée aux autres

            printf("NEW CONNECTION !!!! \n");

            // 1. Sélection d'un thread
            threadArgs * thread = NULL;
            int isCreating = 0;

            if (socketsCount % MAX_SOCKETS_PER_THREAD == 0) {
                // Création
                printf("creating thread %d\n", threadsCount);
                threads[threadsCount] = malloc(sizeof(threadArgs));

                thread = threads[threadsCount];
                thread->sockets = (int*) malloc(MAX_SOCKETS_PER_THREAD * sizeof(int));
                thread->socketsCount = 0;
                pthread_mutex_init(&thread->mutex, NULL);
                pthread_cond_init(&thread->condition, NULL);
                isCreating = 1;
            } else {
                // Assignement du dernier
                printf("adding to thread %d\n", threadsCount-1);
                thread = threads[threadsCount-1];
                isCreating = 0;
            }

            socketsCount++;
//
//            for (i = 0; i < threadsCount; i++) {
//                if (threads[i]->socketsCount < MAX_SOCKETS_PER_THREAD) {
//                    printf("adding to thread %d\n", i);
//                    thread = threads[i];
//                    isCreating = 0;
//                }
//            }
//
//            if (thread == NULL) {
//                thread = malloc(sizeof(threadArgs));
//                printf("creating thread %d\n", threadsCount);
//                isCreating = 1;
//                thread->sockets = (int*) malloc(MAX_SOCKETS_PER_THREAD * sizeof(int));
//                thread->socketsCount = 0;
//                pthread_mutex_init(&thread->mutex, NULL);
//                pthread_cond_init(&thread->condition, NULL);
//            }



        //connectFileDescriptor = acceptConnection(listenFileDescriptor);
            maxSocket = connectFileDescriptor + 1;


           /* incomingSocket = malloc(1);
            * incomingSocket = listenFileDescriptor; */

            printf("socketsCount ==== %d\n", thread->socketsCount);
            thread->sockets[thread->socketsCount] = connectFileDescriptor;

            //sockets = (int *) realloc(sockets, (socketsCount+1) * sizeof(int));
            //sockets[socketsCount] = connectFileDescriptor;

            printf("after segfault11\n");

            // Mise en place de la partie
            int random = randomNumber(0, wordsTotal);
            thread->selectedWords[thread->socketsCount] = wordsList[random];
            printf("random number : %d\n", random);
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

//            printf("Thread address (in main) : %p\n", &thread);
//            printf("SocketsCount value (in main) : %d\n", thread->socketsCount);

            if (isCreating == 1) {
               // if (threadsCount == 0)
//                    pthread_create(&(thread->thread), NULL, gameThread, (void *) &thread);
                    pthread_create(&(thread->thread), NULL, gameThread, (void *) threadsCount++);
                //threads[threadsCount++] = thread;
//                threadsCount++;
            }

            for (i = 0; i < threadsCount; i++) {
                printf("Thread %d addresses : %p\n", i, &(threads[i]->thread));
            }


    }

    close(listenFileDescriptor); // à passer en signal via ctrl-c

    return 0;

}