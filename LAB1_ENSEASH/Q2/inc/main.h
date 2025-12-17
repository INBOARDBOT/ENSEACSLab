/*
* LAB made by JULIEN BURGER and ALEKSANDAR DJOKIC
* LAB 1 ENSEA SH
*/
#ifndef MAIN_H 
#define MAIN_H 

#include <string.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <sys/wait.h>
#include <stdio.h>

// Shell State Logic
#define SHELL_CONTINUE   0
#define SHELL_EXIT_CODE  -1
#define MIN_BYTES_READ   0

// Buffer and Size Limits
#define BUFFER_SIZE      256

// PID REFs
#define CHILD_PID            0
#define WAIT_OPTIONS_DEFAULT 0

// Strings and Characters
const char* shellNameMsg    = "enseash";
const char* shellWelcomeMsg = "Welcome to ENSEA magic Shell.\nType 'exit' to quit.\n";
const char* exitMsg         = "Bye bye...\n";

const char* shellNameSepMsg = " % ";
const char* shellExitCmd    = "exit";

const char  endStringChar     = '\0';
const char* newlineStringChar = "\n";

// Global variables
char shellUserCmd[BUFFER_SIZE];
int shellcmdexit = 0; 

// FUNCTION PROTOTYPES
int main(int argc, char** argv); 
void shellnamePrompt(void);
void shellInit(void);
int shellRunning(void);
int manageUserCmd(const char* cmd);
const char* shellReading(void);
void shellWritePrompt(const char* shellprompt);
int doExitCmdMethod(void);
void executeExternalCommand(const char* cmd);

#endif