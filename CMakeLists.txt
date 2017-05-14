cmake_minimum_required(VERSION 3.7)
project(Hangman)

set(INCLUDES hangman.h hangman.c helper.h helper.c)
set(SERVER server.c ${INCLUDES})
set(LOCAL main.c ${INCLUDES})
set(CLIENT client.c ${INCLUDES})

add_executable(local ${LOCAL})
add_executable(server ${SERVER})
add_executable(client ${CLIENT})