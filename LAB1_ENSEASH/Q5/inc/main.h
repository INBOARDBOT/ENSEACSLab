/*
* LAB made by JULIEN BURGER and ALEKSANDAR DJOKIC
* LAB 1 ENSEA SH
*/
#ifndef MAIN_H // prevent header redifnition in preprocessus 
#define MAIN_H 


#include <string.h> 
#include <unistd.h> 
#include <stdlib.h> 
#include <time.h>
#include <sys/wait.h>
#include <stdio.h>


// Shell State Logic
#define SHELL_CONTINUE   0
#define SHELL_EXIT_CODE  -1
#define MIN_BYTES_READ   0

// Buffer and Size Limits
#define BUFFER_SIZE       256
#define MAX_ARGS          4
#define STATUS_STR_SIZE   16
#define VAL_STR_SIZE      4
#define ASCII_OFFSET_ZERO '0'

// Numerical Constants
#define MS_PER_SEC       1000
#define NS_PER_MS        1000000
#define BASE_10          10
#define HUNDREDS_PLACE   100
#define INITIAL_TIME     0

// PID REFs
#define CHILD_PID            0
#define WAIT_OPTIONS_DEFAULT 0
#define EXIT_SUCCESS_CODE    0


// Strings and Characters
const char* shellNameMsg    = "enseash";
const char* shellWelcomeMsg = "Welcome to ENSEA magic Shell.\nType 'exit' to quit.\n";
const char* unknownCmdMsg   = "Unknown command\n";
const char* exitMsg         = "Bye bye...\n";

const char* shellNameSepMsg = " % ";
const char* shellExitCmd    = "exit";
const char* signPrefix      = "sign";

const char  endStringChar     = '\0';
const char*  newlineStringChar = "\n";
const char* argSeparator      = " ";

// global variables
char shellUserCmd[BUFFER_SIZE];
int shellcmdexit = 0; 
long execProcTime = 0;

// MAIN FUNCTION //
int main(int /*argc*/, char** /*argv*/); 


// SHELL FUNCTIONS //
void shellnamePrompt(void);
void shellInit(void);
int shellRunning(void);
int manageUserCmd(const char* cmd);
const char* shellReading(void);
void shellWritePrompt(const char* shellprompt);

// SHELL CMD FUNCTION //
int doExitCmdMethod(void);
void executeExternalCommand(const char* cmd);

// PROCESS FUNC //
void appendStatus(char *prompt, const char *type, int value, long timeMs);

#endif 