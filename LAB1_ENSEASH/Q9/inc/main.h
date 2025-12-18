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
#include <sys/resource.h>
#include <stdio.h>
#include <fcntl.h>



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
 
// Shell State Logic
#define SHELL_CONTINUE   0
#define SHELL_EXIT_CODE  -1
#define MIN_BYTES_READ   0

// PID REFs
#define CHILD_PID                        0
#define WAIT_OPTIONS_DEFAULT             0
#define EXIT_SUCCESS_CODE                0
#define MAX_JOBS                         10
#define BLOCK_UNTIL_CHILD_FINISHES       0
#define NONBLOCKINGUNTIL_CHILD_FINISHES  WNOHANG

// File descriptor state
#define FILE_DESCRIPTOR_NOT_OPEN -1
#define PERMISION_RIGHT_FILE_DESCRIPTOR 0644

// pipeline
#define PIPE_SYMBOL "|"
#define PIPE_SYMBOL_CHAR '|'

// manage which function to use depending on cmd input
typedef enum {
    GOTO_PIPEMANAGEMENT,
    GOTO_CMDMANAGEMENT
}Redirect_t;

typedef struct {
    pid_t pid;
    char cmdName[BUFFER_SIZE];
    int current_state; 
} Job_t;

// Strings and Characters
const char* shellNameMsg    = "enseash";
const char* shellNameSepMsg = " % ";
const char* shellWelcomeMsg = "Welcome to ENSEA magic Shell.\nType 'exit' to quit.\n";
const char* exitMsg         = "Bye bye...\n";

const char* shellExitCmd    = "exit";
const char* signPrefix      = "sign";

const char  endStringChar      = '\0';
const char*  newlineStringChar = "\n";
const char* argSeparator       = " ";
const char* stdInRedirection   = ">";
const char* stdOutRedirection  = "<";
const char* fdOutErrorMsg      = "enseash: open output";
const char* fdInErrorMsg       = "enseash: open input";
const char* cmdErrorMsg        = "enseash: command not found";

char* inputFile = NULL;  
char* outputFile = NULL; 


// Function Prototypes
int main(int argc, char** argv); 
void shellnamePrompt(void);
void shellInit(void);
int shellRunning(void);
int manageUserCmd(const char* cmd);
const char* shellReading(void);
void shellWritePrompt(const char* shellprompt);
void parseShellUserInput(const char* input);
int doExitCmdMethod(void);
void executeExternalCommand(void);
void appendStatus(char *prompt, const char *type, int value, long timeMs);
void handleRedirection(char *symbol, char *filename);
void manageFileDescriptor(void);
void executePipeCmnd(const char* cmd);
Redirect_t RedirectCmd(const char* cmd);
void checkBackgroundJobs();
void findBckgrndProcessCmd(int* backgroundState);
void actualizeProcessTable(pid_t processID);

#endif