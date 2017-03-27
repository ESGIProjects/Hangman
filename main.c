//
// Created by Jason Pierna on 24/03/2017.
//

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, const char * argv[]) {
    printf("Début\n\n");

    char * words[10];
    char temp = ' ';
    int position = 0;
    int mots = 0;

    // Ouverture du fichier
    int fd = open("/Users/Silver/Desktop/file.txt", O_RDWR);
    if (fd == -1) {
        printf("Erreur lors de l'ouverture\n");
        exit(-1);
    }

    ssize_t bytesRead;
    do {
        bytesRead = read(fd, &temp, sizeof(char));

        if (bytesRead == -1) {
            printf("Erreur lors de la lecture\n");
            exit(-1);
        }
        else if (bytesRead == 0 || bytesRead < sizeof(char)) {
            //printf("Erreur : fin du fichier atteinte\n");
        }
        else {
            if (mots == 0 && position == 0) {
                words[0] = (char *) malloc(sizeof(char) * 30);
            }

            if (temp == '\n') {
                words[mots][position] = '\0';
                words[++mots] = (char *) malloc(sizeof(char) * 30);
                position = 0;
            }
            else {
                words[mots][position++] = temp;
            }
        }
    } while (bytesRead != 0 && bytesRead >= sizeof(char));
    close(fd);


    int i;
    for (i = 0; i <= mots; i++)
        printf("%s\n", words[i]);


    return 0;
}