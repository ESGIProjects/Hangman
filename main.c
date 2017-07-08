//
// Created by Jason Pierna on 24/03/2017.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "libs/hangman.h"

//int main(int argc, const char * argv[]) {
//   int wordsTotal = 0;
//
//    int fileDescriptor = openFile("words.txt");
//    char ** wordsList = readFile(fileDescriptor, &wordsTotal);
//    close(fileDescriptor);
//
//    srand((unsigned int) time(NULL));
//    char * selectedWord = wordsList[randomNumber(0, wordsTotal)];
//
//    runGame(selectedWord);
//
//    return 0;
//}

void removeInteger(int *array, int index, int arrayLength) {
    int i;
    for (i = index; i < arrayLength - 1; i++) array[i] = array[i + 1];
}

void removeString(char **array, int index, int arrayLength) {
    int i;
    for (i = index; i < arrayLength - 1; i++) array[i] = array[i + 1];
}

int main(int argc, const char * argv[]) {
    int i, intArray[5], intArrayLength = 0, stringArrayLength = 0;
    char * stringArray[5];

    intArray[intArrayLength++] = 1;
    intArray[intArrayLength++] = 2;
    intArray[intArrayLength++] = 3;
    intArray[intArrayLength++] = 4;
    intArray[intArrayLength++] = 5;

    stringArray[stringArrayLength++] = "premier";
    stringArray[stringArrayLength++] = "deuxième";
    stringArray[stringArrayLength++] = "troisième";
    stringArray[stringArrayLength++] = "quatrième";
    stringArray[stringArrayLength++] = "cinquième";

    printf("\n\n\nPREMIER AFFICHAGE\n\n\n");

    for (i = 0; i < intArrayLength; i++)
        printf("int : %d\n", intArray[i]);

    for (i = 0; i < stringArrayLength; i++)
        printf("str : %s\n", stringArray[i]);

    printf("\n\n\nUTILISATION DES FONCTIONS DE DECOUPE\n\n\n");

    removeInteger(intArray, 2, intArrayLength--);
    removeString(stringArray, 2, stringArrayLength--);

    printf("\n\n\nSECOND AFFICHAGE\n\n\n");

    for (i = 0; i < intArrayLength; i++)
        printf("int : %d\n", intArray[i]);

    for (i = 0; i < stringArrayLength; i++)
        printf("str : %s\n", stringArray[i]);
}