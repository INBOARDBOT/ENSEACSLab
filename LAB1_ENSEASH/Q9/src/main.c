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

int manageUserCmd(const char* cmd) {
    if (strncmp(cmd, shellExitCmd, strlen(shellExitCmd)) == 0) {
        return doExitCmdMethod();
    }

    char* pipePos = strchr(cmd, PIPE_SYMBOL_CHAR);
    if (pipePos != NULL) {
        executePipeCmnd(cmd);
    } else {
        parseShellUserInput(cmd);
        executeExternalCommand();
    }
    return SHELL_CONTINUE;
}

int shellRunning(void){
    checkBackgroundJobs();
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
    int status;
    struct rusage usage;
    int isBackground = 0;

    // Determine if the command is background (find the last argument)
    int last = 0;
    while (shellCmdArgs[last] != NULL) last++;
    if (last > 0 && strcmp(shellCmdArgs[last-1], "&") == 0) {
        isBackground = 1;
        shellCmdArgs[last-1] = NULL; 
    }

    pid_t pid = fork();
    if (pid == CHILD_PID) {
        execvp(shellCmdArgs[0], shellCmdArgs);
        perror(cmdErrorMsg);
        exit(EXIT_FAILURE);
    } else if (pid > CHILD_PID) {
        if (!isBackground) {
            wait4(pid, &status, 0, &usage);
            shellcmdexit = status;

            long user_ms = (usage.ru_utime.tv_sec * 1000) + (usage.ru_utime.tv_usec / 1000);
            long sys_ms = (usage.ru_stime.tv_sec * 1000) + (usage.ru_stime.tv_usec / 1000);
            execProcTime = user_ms + sys_ms;
        } else {
            for (int j = 0; j < MAX_JOBS; j++) {
                if (!jobs[j].current_state) {
                    jobs[j].pid = pid;
                    strcpy(jobs[j].cmdName, shellCmdArgs[0]);
                    jobs[j].current_state = 1;
                    printf("[%d] %d\n", j + 1, pid);
                    break;
                }
            }
        }
    }
}

void parseShellUserInput(const char *input) {
    int i = 0;
    inputFile = NULL;  
    outputFile = NULL;

    char *token = strtok((char *)input, argSeparator);
    
    while (token != NULL && i < MAX_ARGS) {
        if (strcmp(token, stdInRedirection) == 0 || strcmp(token, stdOutRedirection) == 0) {
            char *symbol = token;
            char *filename = strtok(NULL, argSeparator); 
            handleRedirection(symbol, filename);
        } else {
            shellCmdArgs[i++] = token;
        }
        token = strtok(NULL, argSeparator);
    }
    shellCmdArgs[i] = NULL;
}

void handleRedirection(char *symbol, char *filename) {
    if (symbol == NULL || filename == NULL) return;

    if (strcmp(symbol, stdInRedirection) == 0) {
        outputFile = filename;
    } else if (strcmp(symbol, stdOutRedirection) == 0) {
        inputFile = filename;
    }
}

void executePipeCmnd(const char* cmd) {
    // takes incoming data from both sides of the pipeline
    char* cmdCopy = strdup(cmd);
    char* leftPart = strtok(cmdCopy, PIPE_SYMBOL);
    char* rightPart = strtok(NULL, ""); 

    int pipefd[2];
    if (pipe(pipefd) == -1) { perror("pipe"); return; }

    // Creates the writer process (left side of the pipe)
    pid_t pid1 = fork();
    if (pid1 == 0) {
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[0]); 
        close(pipefd[1]);
        parseShellUserInput(leftPart);
        execvp(shellCmdArgs[0], shellCmdArgs);
        exit(EXIT_FAILURE);
    }

    // Creates the reader process (right side of the pipe)
    pid_t pid2 = fork();
    if (pid2 == 0) {
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[1]);
        close(pipefd[0]);
        parseShellUserInput(rightPart);
        execvp(shellCmdArgs[0], shellCmdArgs);
        exit(EXIT_FAILURE);
    }

    // Prevent issues like zombie process and mem leaks
    close(pipefd[0]);
    close(pipefd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, &shellcmdexit, 0); 

    free(cmdCopy);
}

void checkBackgroundJobs() {
    int status;
    struct rusage usage;
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].current_state) {
            pid_t ret = wait4(jobs[i].pid, &status, WNOHANG, &usage);
            if (ret > 0) {
                printf("\n[%d]+ Ended: %s &\n", i + 1, jobs[i].cmdName);
                jobs[i].current_state = 0;
            }
        }
    }
}