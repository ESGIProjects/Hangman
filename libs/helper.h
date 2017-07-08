//
// Created by Jason Pierna on 13/04/2017.
//
#define PORT 7777

int acceptConnection(int fileDescriptor);
int connectServer(int fileDescriptor, char * serverAddress, int port);
int strsub(const char * input, char * output, int offset, int length);
void deleteIntegerFromArray(int ** sockets, int *socketsCount, int splitPosition);
void removeInteger(int*, int, int);
void removeString(char**, int, int);
