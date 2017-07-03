//
// Created by Jason Pierna on 17/04/2017.
//

#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

struct sockaddr_in getSocketAddress(sa_family_t family, int port) {
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = family;
    serverAddr.sin_port = htons(port);

    return serverAddr;
}

int main(int argc, const char * argv[]) {
    int clientSocket;
    char buffer[1024];
    char str[100];
    struct sockaddr_in serverAddr;
    socklen_t addr_size;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0); // why PF and not AF?

    serverAddr  = getSocketAddress(AF_INET, 7777);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connexion au serveur
    addr_size = sizeof(serverAddr);
    connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);


    int sockmax = clientSocket + 1;
    fd_set rdfs;

    while(1) {
        FD_ZERO(&rdfs);
        FD_SET(clientSocket, &rdfs);

        select(sockmax, &rdfs, NULL, NULL, NULL);

        if (FD_ISSET(clientSocket, &rdfs)) {
            read(clientSocket, buffer, sizeof(buffer));

            printf("Server said: %s \n", buffer);
            printf("Type something: ");
            fgets(str, 100, stdin);

            write(clientSocket, str, sizeof(str));
        }
    }

    close(clientSocket);

    return 0;
}