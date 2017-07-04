//
// Created by Jason Pierna on 13/04/2017.
//

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "hangman.h"

int openFile(const char * path) {
    int fileDescriptor = open(path, O_RDONLY);

    if (fileDescriptor == -1) {
        printf("Error while opening file\n");
        exit(-1);
    }

    return fileDescriptor;
}

char ** readFile(int fileDescriptor, int * wordsTotal) {
    // Internal variables
    char temp = 0;
    int cursor = 0;
    ssize_t bytesRead;

    // Setting up pointers
    char ** wordsList = malloc(10 * sizeof(char*));
    *wordsTotal = 0;

    // Reading inside file
    do {
        bytesRead = read(fileDescriptor, &temp, sizeof(char));

        if (bytesRead == -1) {
            // Error while reading file
            printf("Error while reading file\n");
            exit(-1);
        } else if (bytesRead >= sizeof(char)) {
            if (cursor == 0) {
                // Allocate some memory to catch the word
                wordsList[*wordsTotal] = (char *) malloc(sizeof(char) * 30);
            }

            if (temp == '\n') {
                // Finish the current word and update the word count
                wordsList[*wordsTotal][cursor] = '\0';

                // Reallocate unused memory
                wordsList[*wordsTotal] = (char*)realloc(wordsList[*wordsTotal], (cursor+1)* sizeof(char));

                // Update the word count
                (*wordsTotal)++;

                // Reset the cursor
                cursor = 0;
            } else {
                // Write the letter and update the cursor
                wordsList[*wordsTotal][cursor++] = temp;
            }
        }
    } while (bytesRead >= sizeof(char));

    // Update the word count
    if (*wordsTotal > 0)
        (*wordsTotal)++;

    return wordsList;
}

int randomNumber(int from, int to) {
    return rand() % (to - from) + from;
}

char * getDashedWord(char * word) {
    int i = 0;

    // Get the word length
    size_t wordLength = strlen(word);

    // Allocate same memory to new dashed word
    char * dashedWord = malloc(wordLength * sizeof(char));

    // Dash the word
    for (i = 0; i < strlen(word); i++) {
        dashedWord[i] = '_';
    }

    return dashedWord;
}

void runGame(char * word) {
    int gameRunning = 1, availableTries = 10;
    char letterTyped = 0;

    char * dashedWord = getDashedWord(word);

    do {
        // Ask for a letter
        printf("Type a letter: ");
        scanf("%c", &letterTyped);

        // Check the answer by looping though the word
        int replacedLetters = checkAnswer(letterTyped, word, dashedWord);

        // Check this try's results
        if (replacedLetters == 0) {
            // No letter has been replaced: wrong answer
            availableTries--;
            printf("Wrong guess!\n");
        } else {
            // Good guess, display the dashed word updated
            printf("Good guess!\n%s\n", dashedWord);
        }

        // Check the game state
        if (strcmp(dashedWord, word) == 0) {
            gameRunning = 0;
            printf("You win!");
        }

        if (availableTries == 0) {
            gameRunning = 0;
            printf("No more tries, you lose! The word was %s\n", word);
        }

    } while (gameRunning == 1);
}

int checkAnswer(char letterTyped, char * word, char * dashedWord) {
    size_t wordLength = strlen(word);
    int replacedLetters = 0;

    // Check the answer by looping though the word
    for (int i = 0; i < wordLength; i++) {
        if (word[i] == letterTyped) {
            // The letter exists: does it need to be replaced?
            if (dashedWord[i] == '_') {
                // Replace the letter
                dashedWord[i] = letterTyped;
                replacedLetters++;
            }
        }
    }

    return replacedLetters;
}