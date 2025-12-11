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
    if (strcmp(cmd, shellFortuneCmd) == 0) {
        doFortuneCmdMethod();
    } else if (strcmp(cmd, shellDateCmd) == 0) {
        doDateCmdMethod();
    } else if (strcmp(cmd, shellExitCmd) == 0) {
        doExitCmdMethod();
        return -1; 
    } else {
        unknownCmdPrompt();
    }
    return 0; 
}


int shellRunning(void){
    shellnamePrompt();
    const char* shellUserCmd = shellReading();
    if(shellUserCmd == NULL)return -1;

    int shellState = manageUserCmd(shellUserCmd);

    return shellState;
}

void shellInit(void){
    welcomePrompt();
}

const char* shellReading(void){

    ssize_t bytesRead = read(STDIN_FILENO, shellUserCmd, BUFFER_SIZE - 1);
    if (bytesRead <= 0) {
        return NULL;
    }

    shellUserCmd[bytesRead] = '\0'; // Terminer la chaîne
    shellUserCmd[strcspn(shellUserCmd, "\n")] = '\0'; // Supprimer le \n
    return (const char*)shellUserCmd;
}

// SHELL PROMPTS //
void welcomePrompt(void){
    write(STDOUT_FILENO, shellWelcomeMsg, strlen(shellWelcomeMsg));
}

void shellnamePrompt(void){
    write(STDOUT_FILENO, shellNameMsg, strlen(shellNameMsg));
}

void exitPrompt(void){
    write(STDOUT_FILENO, exitMsg, strlen(exitMsg));
}

void unknownCmdPrompt(void){
    write(STDOUT_FILENO, unknownCmdMsg, strlen(unknownCmdMsg));
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
    write(STDOUT_FILENO, fortuneMsg, strlen(fortuneMsg));
}

void doExitCmdMethod(void){
    write(STDOUT_FILENO, "Bye bye...\n", 11);
}