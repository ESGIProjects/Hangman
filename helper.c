//
// Created by Jason Pierna on 13/04/2017.
//

#include <stdio.h>
#include <sys/socket.h>
#include "helper.h"

int acceptConnection(int fileDescriptor) {
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;

    return accept(fileDescriptor, (struct sockaddr*) &serverStorage, &addr_size);
}