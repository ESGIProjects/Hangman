//
// Created by Jason Pierna on 13/04/2017.
//

#ifndef HANGMAN_HANGMAN_H
#define HANGMAN_HANGMAN_H

int openFile(const char * path);
char ** readFile(int fileDescriptor, int * wordsTotal);
int randomNumber(int from, int to);
char * getDashedWord(char * word);
void runGame(char * word);

#endif //HANGMAN_HANGMAN_H
