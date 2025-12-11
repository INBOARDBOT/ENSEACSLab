/*
* LAB made by JULIEN BURGER and ALEKSANDAR DJOKIC
* LAB 1 ENSEA SH
*/
#ifndef MAIN_H // prevent header redifnition in preprocessus 
#define MAIN_H // define the only MAIN_H header

#include <string.h> // useful for memory management
#include <unistd.h> // access to POSIX OS API
#include <stdlib.h> // basic utilities for C programming
#include <time.h>   // for any time related task

#define BUFFER_SIZE 256 // maximum of size of input char by user in STDIN_FILENO

// const char* (string) magic shell catchphrase 
#define shellName "enseash % "
#define shellWelcomeMsg "Welcome to ENSEA magic Shell.\nType 'exit' to quit.\n"


// Command types
const char* FORTUNECMD = "fortune";
const char* DATECMD = "date";
const char* EXITCMD = "exit";

int last_exit_status; // Global variable for exit status
// main function called by the compiler
int main(int argc, char *argv[]); // definition of main method

// function handling differnet functions depending of user's input
int handleCommand(const char* buffer);

// Command functions
void doDateCmdMethod(void);
void doFortuneCmdMethod(void);
void doExitCmdMethod(void);

// manage shell's prompt
void printPrompt(void);

#endif // end of heqder definition