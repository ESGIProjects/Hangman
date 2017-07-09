//
// Created by Jason Pierna on 13/04/2017.
//

#define PORT 7770

typedef struct threadArgs {
    pthread_t thread;
    int * sockets;
    int socketsCount;

    // Variables relatives au jeu

    char * selectedWords[5], * dashedWords[5];
    int availableTries[5];

    pthread_mutex_t mutex;
    pthread_cond_t condition;
} threadArgs;

int acceptConnection(int fileDescriptor);
int connectServer(int fileDescriptor, char * serverAddress, int port);
int strsub(const char * input, char * output, int offset, int length);
void deleteIntegerFromArray(int ** sockets, int *socketsCount, int splitPosition);
void removeInteger(int * array, int index, int arrayLength);
void removeString(char ** array, int index, int arrayLength);
void removeThreads(threadArgs * array, int index, int arrayLength);
