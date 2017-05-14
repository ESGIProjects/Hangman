//
// Created by Jason Pierna on 24/03/2017.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "hangman.h"

int main(int argc, const char * argv[]) {
    int wordsTotal = 0;

    int fileDescriptor = openFile("words.txt");
    char ** wordsList = readFile(fileDescriptor, &wordsTotal);
    close(fileDescriptor);

    srand((unsigned int) time(NULL));
    char * selectedWord = wordsList[randomNumber(0, wordsTotal)];

    runGame(selectedWord);

    return 0;
}