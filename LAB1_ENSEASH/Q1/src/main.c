#include "main.h"



int main(int /*argc*/, char** /*argv*/) {

    int shellState;
    shellInit();

    while (1) {
        shellState = shellRunning();
        if(shellState == -1)break;
    }

    return EXIT_SUCCESS;
}

// SHELL FUNCTIONS //

int manageUserCmd(const char* cmd){
    if (strncmp(cmd, "exit", 4) == 0) {    
        return -1;
    }
    return 0;
}


int shellRunning(void){
    shellnamePrompt();
    ssize_t bytesRead = read(STDIN_FILENO, shellUserCmd, BUFFER_SIZE - 1);
    if (bytesRead <= 0) {
        return -1;
    }
    int shellState = manageUserCmd(shellUserCmd);

    return shellState;
}

void shellInit(void){
    welcomePrompt();
}

// SHELL PROMPTS //

void welcomePrompt(void){
    write(STDOUT_FILENO, shellWelcomeMsg, strlen(shellWelcomeMsg));
}

void shellnamePrompt(void){
    write(STDOUT_FILENO, shellName, strlen(shellName));
}

void exitPrompt(void){
    write(STDOUT_FILENO, "Bye bye...\n", 11);
}