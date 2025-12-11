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
    if (strncmp(cmd, shellFortuneCmd, strlen(shellFortuneCmd)) == 0) {
        doFortuneCmdMethod();
        shellcmdexit = 0;
        execProcTime = 0;
    } else if (strncmp(cmd, shellDateCmd, strlen(shellDateCmd)) == 0) {
        doDateCmdMethod();
        shellcmdexit = 0;
        execProcTime = 0;
    } else if (strncmp(cmd, shellExitCmd, strlen(shellExitCmd)) == 0) {
        doExitCmdMethod();
        shellcmdexit = 0;
        execProcTime = 0;
        return -1; 
    } else {
        executeExternalCommand(cmd);
    }
    return 0; 
}

// SHELL FUNCTIONS //
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

// SHELL PROMPTS //
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


// SHELL CMD FUNCTION //
void doDateCmdMethod(void){
    time_t now;
    struct tm *local_time;

    // Save old TZ
    char *old_tz = getenv("TZ");

    // Set to CET
    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
    tzset();

    time(&now);
    local_time = localtime(&now);

    char buffer[64];
    int len = strftime(buffer, sizeof(buffer), "%a %b %d %H:%M:%S %Y\n", local_time);
    write(STDOUT_FILENO, buffer, len);

    // Restore old TZ
    if (old_tz) {
        setenv("TZ", old_tz, 1);
    } else {
        unsetenv("TZ");
    }
    tzset();
}

void doFortuneCmdMethod(void){
    shellWritePrompt(fortuneMsg);
}

void doExitCmdMethod(void){
    shellWritePrompt(exitMsg);
}

void executeExternalCommand(const char* cmd) {
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);
    
    pid_t pid = fork();
    if (pid == 0) {
        execlp(cmd, cmd, (char*)NULL);
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