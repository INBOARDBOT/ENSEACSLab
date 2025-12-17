#include "main.h"


int main(int /*argc*/, char** /*argv*/) {
    shellInit();
    while (1) {
        if (shellRunning() == SHELL_EXIT_CODE) {
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
    return SHELL_CONTINUE; 
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

const char* shellReading(void) {
    ssize_t bytesRead = read(STDIN_FILENO, shellUserCmd, BUFFER_SIZE - 1);
    
    if (bytesRead <= MIN_BYTES_READ) {
        return shellExitCmd;
    }

    shellUserCmd[bytesRead] = endStringChar;
    shellUserCmd[strcspn(shellUserCmd, newlineStringChar)] = endStringChar;
    return (const char*)shellUserCmd;
}

void shellWritePrompt(const char* shellprompt){
    write(STDOUT_FILENO, shellprompt, strlen(shellprompt));
}

int doExitCmdMethod(void){
    shellWritePrompt(exitMsg);
    return SHELL_EXIT_CODE;
}

void shellnamePrompt(void){
    char prompt[BUFFER_SIZE];

    // strcpy because cant assign const char * fixed char[] 
    strcpy(prompt, shellNameMsg);

    if (WIFEXITED(shellcmdexit)) {
        appendStatus(prompt, shellExitCmd, WEXITSTATUS(shellcmdexit), execProcTime);
    } else if (WIFSIGNALED(shellcmdexit)) {
        appendStatus(prompt, signPrefix, WTERMSIG(shellcmdexit), execProcTime);
    }

    strcat(prompt, shellNameSepMsg);
    shellWritePrompt(prompt);
}
 
void appendStatus(char *prompt, const char *type, int value, long timeMs) {
    char valStr[VAL_STR_SIZE];
    
    if (value >= HUNDREDS_PLACE) {
        valStr[0] = ASCII_OFFSET_ZERO + (value / HUNDREDS_PLACE);
        valStr[1] = ASCII_OFFSET_ZERO + ((value % HUNDREDS_PLACE) / BASE_10);
        valStr[2] = ASCII_OFFSET_ZERO + (value % BASE_10);
        valStr[3] = endStringChar;
    } else if (value >= BASE_10) {
        valStr[0] = ASCII_OFFSET_ZERO + (value / BASE_10);
        valStr[1] = ASCII_OFFSET_ZERO + (value % BASE_10);
        valStr[2] = endStringChar;
    } else {
        valStr[0] = ASCII_OFFSET_ZERO + value;
        valStr[1] = endStringChar;
    }

    char timeStr[STATUS_STR_SIZE];
    sprintf(timeStr, "%s:%s|%ldms", type, valStr, timeMs);
    strcat(prompt, " [");
    strcat(prompt, timeStr);
    strcat(prompt, "]");
}

void executeExternalCommand(void) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    pid_t pid = fork();
    if (pid == CHILD_PID) {
        execvp(shellCmdArgs[0], shellCmdArgs);
        exit(EXIT_FAILURE); 
    } else if (pid > CHILD_PID) {
        int status;
        waitpid(pid, &status, WAIT_OPTIONS_DEFAULT);
        shellcmdexit = status;
        clock_gettime(CLOCK_MONOTONIC, &end);

        long seconds = end.tv_sec - start.tv_sec;
        long nanoseconds = end.tv_nsec - start.tv_nsec;
        execProcTime = (seconds * MS_PER_SEC) + (nanoseconds / NS_PER_MS);
    } else {
        perror("fork");
        shellcmdexit = SHELL_EXIT_CODE;
        execProcTime = INITIAL_TIME;
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