#include "main.h"


int main(int /*argc*/, char** /*argv*/) {

    int shellState;
    shellInit();

    while (1) {
        shellState = shellRunning();
        if(shellState == -1){
            break;
        }
    }
    return EXIT_SUCCESS;
}

////////////////////////////////////////////////////////////

int manageUserCmd(const char* cmd){

    if (strncmp(cmd, shellExitCmd, strlen(shellExitCmd)) == 0) {
        int exit_state = doExitCmdMethod();
        shellcmdexit = 0;
        execProcTime = 0;
        return exit_state; 
    } else {
        parseShellUserInput(cmd);
        executeExternalCommand();
    }
    return 0; 
}

int shellRunning(void){
    shellnamePrompt();
    const char* shellUserCmd = shellReading();

    int shellState = manageUserCmd(shellUserCmd);

    return shellState;
}

void shellInit(void){
    shellWritePrompt(shellWelcomeMsg);
}

const char* shellReading(void){

    ssize_t bytesRead = read(STDIN_FILENO, shellUserCmd, BUFFER_SIZE - 1);
    if (bytesRead <= 0) {
        return shellExitCmd;
    }

    shellUserCmd[bytesRead] = '\0'; // Terminer la chaîne
    shellUserCmd[strcspn(shellUserCmd, "\n")] = '\0'; // Supprimer le \n
    return (const char*)shellUserCmd;
}

void shellWritePrompt(const char* shellprompt){
    write(STDOUT_FILENO, shellprompt, strlen(shellprompt));
}


void shellnamePrompt(void){
    char prompt[BUFFER_SIZE] = "enseash";

    if (WIFEXITED(shellcmdexit)) {
        appendStatus(prompt, "exit", WEXITSTATUS(shellcmdexit), execProcTime);
    } else if (WIFSIGNALED(shellcmdexit)) {
        appendStatus(prompt, "sign", WTERMSIG(shellcmdexit), execProcTime);
    }

    strcat(prompt, " % ");
    shellWritePrompt(prompt);
}

void appendStatus(char *prompt, const char *type, int value, long timeMs) {
    char valStr[4];
    if (value >= 100) {
        valStr[0] = '0' + (value / 100);
        valStr[1] = '0' + ((value % 100) / 10);
        valStr[2] = '0' + (value % 10);
        valStr[3] = '\0';
    } else if (value >= 10) {
        valStr[0] = '0' + (value / 10);
        valStr[1] = '0' + (value % 10);
        valStr[2] = '\0';
    } else {
        valStr[0] = '0' + value;
        valStr[1] = '\0';
    }

    char timeStr[16];
    sprintf(timeStr, "%s:%s|%ldms", type, valStr, timeMs);
    strcat(prompt, " [");
    strcat(prompt, timeStr);
    strcat(prompt, "]");
}

int doExitCmdMethod(void){
    shellWritePrompt(exitMsg);
    return -1;
}

void executeExternalCommand(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    pid_t pid = fork();
    if (pid == 0) {
        execvp(shellCmdArgs[0], shellCmdArgs);
        exit(EXIT_FAILURE); 
    } else if (pid > 0) {
        int status;
        waitpid(pid, &status, 0);
        shellcmdexit = status;
        clock_gettime(CLOCK_MONOTONIC, &end);

        long seconds = end.tv_sec - start.tv_sec;
        long nanoseconds = end.tv_nsec - start.tv_nsec;
        execProcTime = seconds * 1000 + nanoseconds / 1000000;
    } else {
        perror("fork");
        shellcmdexit = -1;
        execProcTime = 0;
    }
}

void parseShellUserInput(const char *input) {
    int i = 0;
    char *token = strtok((char *)input, " ");
    while (token != NULL && i < MAX_ARGS) {
        shellCmdArgs[i++] = token;
        token = strtok(NULL, " ");
    }
    shellCmdArgs[i] = NULL;
}