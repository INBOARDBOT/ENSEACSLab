#ifndef MAIN_H // prevent header redifnition in preprocessus 
#define MAIN_H // define the only MAIN_H header

#include <string.h> // useful for memory management
#include <unistd.h> // access to POSIX OS API
#include <stdlib.h> // basic utilities for C programming


#define BUFFER_SIZE 256 // maximum of size of input char by user in STDIN_FILENO

int main(int argc, char *argv[]); // definition of main method

#endif // end of heqder definition