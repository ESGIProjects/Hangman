//
// Created by Jason Pierna on 13/04/2017.
//

int openFile(const char * path);
char ** readFile(int fileDescriptor, int * wordsTotal);
int randomNumber(int from, int to);
char * getDashedWord(char * word);
void runGame(char * word);
int checkAnswer(char letterTyped, char * word, char * dashedWord);