/*
* LAB made by JULIEN BURGER and ALEKSANDAR DJOKIC
* LAB 1 ENSEA SH
*/
#ifndef MAIN_H // prevent header redifnition in preprocessus 
#define MAIN_H 


#include <string.h> 
#include <unistd.h> 
#include <stdlib.h> 

#define BUFFER_SIZE 256

// const char* (string) magic shell prompt 
const char* shellName = "enseash % ";
const char* shellWelcomeMsg = "Welcome to ENSEA magic Shell.\nType 'exit' to quit.\n";

// max buffer size of incoming user cmd
char shellUserCmd[BUFFER_SIZE];

// MAIN FUNCTION //
int main(int /*argc*/, char** /*argv*/); 

// SHELL PROMPTS //
void shellnamePrompt(void);
void exitPrompt(void);

// SHELL FUNCTIONS //
void shellInit(void);
int shellRunning(void);
int manageUserCmd(const char* cmd);

#endif 