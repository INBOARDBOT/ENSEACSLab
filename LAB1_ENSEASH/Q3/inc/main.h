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

#define BUFFER_SIZE 256

// const char* (string) magic shell prompt 
const char* shellNameMsg    = "enseash % ";
const char* shellWelcomeMsg = "Welcome to ENSEA magic Shell.\nType 'exit' to quit.\n";
const char* unknownCmdMsg   = "Unknown command\n";
const char* exitMsg         = "Bye bye...\n";
const char* fortuneMsg      = "You will have 20/20 at this lab\n";

// Shell command types
const char* shellFortuneCmd = "fortune";
const char* shellDateCmd = "date";
const char* shellExitCmd = "exit";

// Buffer of user input cmd
char shellUserCmd[BUFFER_SIZE];

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

// SHELL CMD FUNCTION //
void doDateCmdMethod(void);
void doFortuneCmdMethod(void);
void doExitCmdMethod(void);

#endif 