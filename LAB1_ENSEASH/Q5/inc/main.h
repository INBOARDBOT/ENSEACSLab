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

#define BUFFER_SIZE 256

// const char* (string) magic shell prompt 
const char* shellNameMsg    = "enseash % ";
const char* shellWelcomeMsg = "Welcome to ENSEA magic Shell.\nType 'exit' to quit.\n";
const char* unknownCmdMsg   = "Unknown command\n";
const char* exitMsg         = "Bye bye...\n";
const char* fortuneMsg      = "You will have 20/20 at this lab\n";

// Magic Shell command types
const char* shellFortuneCmd = "fortune";
const char* shellDateCmd    = "date";
const char* shellExitCmd    = "exit";

// global variables
char shellUserCmd[BUFFER_SIZE];
int shellcmdexit = 0; 
long execProcTime = 0;

// MAIN FUNCTION //
int main(int /*argc*/, char** /*argv*/); 

// SHELL PROMPTS //
void welcomePrompt(void);
void shellnamePrompt(void);
void exitPrompt(void);
void unknownCmdPrompt(void);

// SHELL FUNCTIONS //
void shellInit(void);
int shellRunning(void);
int manageUserCmd(const char* cmd);
const char* shellReading(void);
void shellWritePrompt(const char* shellprompt);

// SHELL CMD FUNCTION //
void doDateCmdMethod(void);
void doFortuneCmdMethod(void);
void doExitCmdMethod(void);
void executeExternalCommand(const char* cmd);

// PROCESS FUNC //
void appendStatus(char *prompt, const char *type, int value, long timeMs);

#endif 