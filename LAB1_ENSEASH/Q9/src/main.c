#include "main.h"

// Global variables
char shellUserCmd[BUFFER_SIZE];
char* shellCmdArgs[MAX_ARGS + 1];
Job_t jobs[MAX_JOBS] = {0};
int shellcmdexit = 0; 
long execProcTime = 0;



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

    Redirect_t redirectState = RedirectCmd(cmd);
    
    switch(redirectState){
        case GOTO_PIPEMANAGEMENT:
            executePipeCmnd(cmd);
            break;
        case GOTO_CMDMANAGEMENT:
            parseShellUserInput(cmd);
            executeExternalCommand();
            break;
        default:
            break;
    }

    return SHELL_CONTINUE;
}

int shellRunning(void){
    checkBackgroundJobs(); // Poll for finished background tasks
    shellnamePrompt();
    const char* userInput = shellReading();
    
    if (strlen(userInput) == 0) return SHELL_CONTINUE;

    return manageUserCmd(userInput);
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

    findBckgrndProcessCmd(&isBackground);

    pid_t pid = fork();
    if (pid == CHILD_PID) {
        manageFileDescriptor(); 
        execvp(shellCmdArgs[0], shellCmdArgs);
        perror(cmdErrorMsg);
        exit(EXIT_FAILURE);
    } else if (pid > CHILD_PID) {
        if (!isBackground) {
            wait4(pid, &status, BLOCK_UNTIL_CHILD_FINISHES, &usage);
            shellcmdexit = status;
            
            long user_ms = (usage.ru_utime.tv_sec * 1000) + (usage.ru_utime.tv_usec / 1000);
            long sys_ms = (usage.ru_stime.tv_sec * 1000) + (usage.ru_stime.tv_usec / 1000);
            execProcTime = user_ms + sys_ms;
        } else {
            actualizeProcessTable(pid);
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

void manageFileDescriptor(void){
    if (outputFile == NULL && inputFile == NULL){
        return;
    }

    if(outputFile != NULL){
        int fdOut = open(outputFile, O_WRONLY | O_CREAT | O_TRUNC, PERMISION_RIGHT_FILE_DESCRIPTOR);
        if (fdOut == FILE_DESCRIPTOR_NOT_OPEN) {
            perror(fdOutErrorMsg);
            exit(EXIT_FAILURE);
        }
        dup2(fdOut, STDOUT_FILENO); 
        close(fdOut);
    }
               
    if(inputFile != NULL){
        int fdIn = open(inputFile, O_RDONLY);
        if (fdIn == FILE_DESCRIPTOR_NOT_OPEN) {
            perror(fdInErrorMsg);
            exit(EXIT_FAILURE);
        }
        dup2(fdIn, STDIN_FILENO);   
        close(fdIn);    
    }            
}

Redirect_t RedirectCmd(const char* cmd){
    char* pipePos = strchr(cmd, PIPE_SYMBOL_CHAR);
    return pipePos != NULL ? GOTO_PIPEMANAGEMENT : GOTO_CMDMANAGEMENT;
}

void checkBackgroundJobs() {
    int status;
    struct rusage usage;
    for (int i = 0; i < MAX_JOBS; i++) {
        if (jobs[i].current_state) {
    
            pid_t ret = wait4(jobs[i].pid, &status, NONBLOCKINGUNTIL_CHILD_FINISHES, &usage);
            
            if (ret > 0) { 
                printf("\n[%d]+ Ended: %s &\n", i + 1, jobs[i].cmdName);
                jobs[i].current_state = 0;
                
                shellcmdexit = status;
                long user_ms = (usage.ru_utime.tv_sec * 1000) + (usage.ru_utime.tv_usec / 1000);
                long sys_ms = (usage.ru_stime.tv_sec * 1000) + (usage.ru_stime.tv_usec / 1000);
                execProcTime = user_ms + sys_ms;
            }
        }
    }
}

void findBckgrndProcessCmd(int* backgroundState){
    int last = 0;
    while (shellCmdArgs[last] != NULL) last++;
    if (last > 0 && strcmp(shellCmdArgs[last-1], "&") == 0) {
        *backgroundState = 1;
        shellCmdArgs[last-1] = NULL; 
    }
}

void actualizeProcessTable(pid_t processID){
    for (int j = 0; j < MAX_JOBS; j++) {
        if (!jobs[j].current_state) {
            jobs[j].pid = processID;
            strncpy(jobs[j].cmdName, shellCmdArgs[0], BUFFER_SIZE);
            jobs[j].current_state = 1;
            printf("[%d] %d\n", j + 1, processID); 
            break;
        }
    }
}