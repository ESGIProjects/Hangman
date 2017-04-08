//
// Created by Jason Pierna on 24/03/2017.
//

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int randomNumber(int from, int to);

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


    // Sélection du mot aléatoirement
    srand((unsigned int)time(NULL));
    int randomChoice = randomNumber(0, mots+1);

    printf("\nrandom: %d\n", randomChoice);
    printf("%s\n", words[randomChoice]);

    // Reconstitution de la chaîne de caractère cachée
    temp = ' ';
    char hidden[30];
    i = 0;
    do {
        temp = words[randomChoice][i];
        if (temp != '\0') {
            hidden[i++] = '_';
        }
        else {
            hidden[i++] = '\0';
        }
    } while (temp != '\0');

    printf("%s\n", hidden);

    int game = 1; // Game is on
    int chances = 6;
    char try;

    do {
        printf("%s\n", hidden);
        printf("Entrez une lettre : ");
        scanf("%c%*c", &try);


        // test si la lettre est correcte ou non
            // Si oui : on change la string hidden
            // Si non : on diminue "chances"

        // test de fin de jeu
            // Est-ce que toutes les lettres sont trouvées ?
            // Est-ce que chances est à 0 ?

    } while (game == 1);


    return 0;
}

int randomNumber(int from, int to) {
    return rand() % (to - from) + from;
}