//
// Created by Jason Pierna on 13/04/2017.
//

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include "helper.h"

int acceptConnection(int fileDescriptor) {
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    return accept(fileDescriptor, (struct sockaddr *) &serverStorage, &addr_size);
}

int connectServer(int fileDescriptor, char * serverAddress, int port) {
    struct sockaddr_in serverInfo;

    serverInfo.sin_family = AF_INET;
    serverInfo.sin_port = htons(port);
    serverInfo.sin_addr.s_addr = inet_addr(serverAddress);

    return connect(fileDescriptor, (struct sockaddr *) &serverInfo, sizeof(serverInfo));
}

int strsub(const char * input, char * output, int offset, int length) {
    size_t inputLength = strlen(input);

    if (offset + length > inputLength)
        return -1;

    strncpy(output,input + offset, length);

    return 0;
}

void deleteIntegerFromArray(int ** sockets, int * socketsCount, int splitPosition) {
    for (int i = splitPosition + 1; i < *socketsCount; i++)
        (*sockets)[i-1] = (*sockets)[i];

    *sockets = (int *) realloc(*sockets, (*socketsCount - 1) * sizeof(int));
    (*socketsCount)--;
}

void removeInteger(int *array, int index, int arrayLength) {
    int i;
    for (i = index; i < arrayLength - 1; i++) array[i] = array[i + 1];
}

void removeString(char **array, int index, int arrayLength) {
    int i;
    for (i = index; i < arrayLength - 1; i++) array[i] = array[i + 1];
}