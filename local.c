//
// Created by Jason Pierna on 24/03/2017.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "libs/hangman.h"

/**
* Ce fichier est utilisé afin de tester les fonctions
* écrites dans la librairie hangman.h.
* Il fonctionne comme un jeu du pendu en local.
*
* La fonction runGame propose un déroulement du jeu sommaire,
* et n'est pas aussi descriptive envers l'utilisateur que le
* client final.
**/

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
