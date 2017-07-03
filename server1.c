//
// Created by Jason Pierna on 17/04/2017.
//

#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <strings.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, const char * argv[]) {
    char str[100];
    int listen_fd, conn_fd;
    struct sockaddr_in servaddr;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr)); // what is bzero

    servaddr.sin_family = AF_INET; // rechercher signification exacte
    servaddr.sin_addr.s_addr = htons(INADDR_ANY); // Toutes les interfaces 192.168.0.1
    servaddr.sin_port = htons(7777); // what is htons

    bind(listen_fd, (struct sockaddr *) &servaddr, sizeof(servaddr)); // ??

    if (!listen(listen_fd, 5)) { // what does listen returns?
        printf("Listening\n");
    } else {
        printf("Error\n");
    }


    int sockmax = listen_fd + 1;
    int allsockets[10];
    fd_set rdfs;
    int allSocketsTotal = 1;
    allsockets[0] = listen_fd;

    while (1) {
        FD_ZERO(&rdfs);
        for (int i = 0; i < allSocketsTotal; i++) {
            FD_SET(allsockets[i], &rdfs);
        }

        select(sockmax, &rdfs, NULL, NULL, NULL);

        if (FD_ISSET(allsockets[0], &rdfs)) {
            struct sockaddr_storage serverStorage; // ??
            socklen_t addr_size; // ??

            addr_size = sizeof(serverStorage);
            conn_fd = accept(listen_fd, (struct sockaddr *) &serverStorage, &addr_size);
            sockmax = conn_fd + 1;
            allsockets[allSocketsTotal++] = conn_fd;

            strcpy(str, "Hello toi!\n");
            write(conn_fd, str, sizeof(str));
    }

        for (int i = 1; i < allSocketsTotal; i++) {
            if (FD_ISSET(allsockets[i], &rdfs)) {
                recv(allsockets[i], str, 100, 0);
                printf("Client no. %d said: %s", allsockets[i], str);
                write(allsockets[i], "Coucou", sizeof("Coucou"));
            }
        }
    }

    close(listen_fd);

    return 0;
}