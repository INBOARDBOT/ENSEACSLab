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


int manageUserCmd(const char* cmd){
    if (strncmp(cmd, shellExitCmd, strlen(shellExitCmd)) == 0) {
        int exit_status = doExitCmdMethod();
        shellcmdexit = 0;
        return exit_status; 
    } else {
        executeExternalCommand(cmd);
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

const char* shellReading(void){
    ssize_t bytesRead = read(STDIN_FILENO, shellUserCmd, BUFFER_SIZE - 1);
    if (bytesRead <= MIN_BYTES_READ) {
        return shellExitCmd;
    }

    shellUserCmd[bytesRead] = endStringChar; // Terminer la chaîne
    shellUserCmd[strcspn(shellUserCmd, newlineStringChar)] = endStringChar; // Supprimer le \n
    return (const char*)shellUserCmd;
}

void shellWritePrompt(const char* shellprompt){
    write(STDOUT_FILENO, shellprompt, strlen(shellprompt));
}

void shellnamePrompt(void){
    char prompt[BUFFER_SIZE];

    if (WIFEXITED(shellcmdexit)) {
        snprintf(prompt, sizeof(prompt), "enseash [exit:%d]", WEXITSTATUS(shellcmdexit));
    } else if (WIFSIGNALED(shellcmdexit)) {
        snprintf(prompt, sizeof(prompt), "enseash [sign:%d]", WTERMSIG(shellcmdexit));
    } else {
        snprintf(prompt, sizeof(prompt), "%s", shellNameMsg);
    }

    strcat(prompt, shellNameSepMsg);
    shellWritePrompt(prompt);
}

int doExitCmdMethod(void){
    shellWritePrompt(exitMsg);
    return SHELL_EXIT_CODE;
}

void executeExternalCommand(const char* cmd) {
    pid_t pid = fork();
    if (pid == CHILD_PID) {
        execlp(cmd, cmd, (char*)NULL);
        exit(EXIT_FAILURE); 
    } else if (pid > CHILD_PID) {
        int status;
        waitpid(pid, &status, WAIT_OPTIONS_DEFAULT);
        shellcmdexit = status;
    } else {
        perror("fork");
        shellcmdexit = -1;
    }
}