#include "main.h"


int last_exit_status = 0; // setup of globql vqriqble 

int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE]; // buffer memory to store incoming user's instruction(s) max BUFFER_SIZE times bytes
    ssize_t bytes_read;       // single byte read (8 bits)

    /*
    * writes shellWelcomeMsg an uint8_t pointer of size strlen(shellWelcomeMsg) to file descriptor
    * STDOUT_FILENO : stand for POSIX's standard output screen
    */ 
    write(STDOUT_FILENO, shellWelcomeMsg, strlen(shellWelcomeMsg)); 

    while (1) {
        printPrompt(); // Show prompt with last exit status
        bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
        if (bytes_read <= 0) { // EOF (Ctrl+D) or error
            doExitCmdMethod();
            break;
        }
        buffer[bytes_read] = '\0'; // Null-terminate
        if (buffer[bytes_read - 1] == '\n')
            buffer[bytes_read - 1] = '\0'; // Remove trailing newline

        if (handleCommand(buffer) == 1)
            break; // Exit if "exit" command
}
    


    // while (1) {
    //     // Advanced prompt display
    //     printPrompt();
        
    //     /*
    //     * Reads incoming user's information(s) as uint8_t pointer of max size BUFFER_SIZE
    //     * Blocks until it receives user's data
    //     * STDIN_FILENO : stand for POSIX's standard input keyboard
    //     */ 
    //     bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE);
    //     buffer[bytes_read] = '\0'; // Ensure null-termination


    //     // Ctrl + d return EOF signal (meaning 0), (also prevent null sized incoming data)
    //     if (bytes_read <= 0){
    //         doExitCmdMethod();
    //         break;
    //     } 

    //     if (handleCommand(buffer) == 1 ) {
    //         break; // Exit the loop if handleCommand returns 1
    //     }
    // }

    // is returned when everything is going wonderfully well
    return EXIT_SUCCESS;
}
/*
* strncmp() function from string lib compare n-th first bytes of buffer char pointer
* with given reference "exit".
* 3 cases are possible either each string compared are equal or inferior or superior 
* to one another, if they are equal it returns 0 (what we want to verify).
*/
int handleCommand(const char* buffer) {
    if (strcmp(buffer, FORTUNECMD) == 0) {
        doFortuneCmdMethod();
        last_exit_status = 0; // Success
    } else if (strcmp(buffer, DATECMD) == 0) {
        doDateCmdMethod();
        last_exit_status = 0; // Success
    } else if (strcmp(buffer, EXITCMD) == 0) {
        doExitCmdMethod();
        last_exit_status = 0; // Success
        return 1; // Signal to exit
    } else {
        // Execute external command
        pid_t pid = fork();
        if (pid == 0) {
            execlp(buffer, buffer, (char *)NULL);
            exit(EXIT_FAILURE);
        } else if (pid > 0) {
            waitpid(pid, &last_exit_status, 0);
        } else {
            write(STDOUT_FILENO, "Fork failed\n", 11);
        }
    }
    return 0; 
}


/*
* Check on user request to display current date for time zone CET
*/
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

/*
* Check on user request to display fortune teller
*/
void doFortuneCmdMethod(void){
    write(STDOUT_FILENO, "You will have 20/20 at this lab\n", 32);
}

/*
* Check on user request to exit
*/
void doExitCmdMethod(void){
    write(STDOUT_FILENO, "Bye bye...\n", 11);
}

/*
* 
*
*/
void printPrompt(void) {
    char prompt[BUFFER_SIZE];
    if (WIFEXITED(last_exit_status)) {
        snprintf(prompt, BUFFER_SIZE, "enseash [exit:%d] %% ", WEXITSTATUS(last_exit_status));
    } else if (WIFSIGNALED(last_exit_status)) {
        snprintf(prompt, BUFFER_SIZE, "enseash [sign:%d] %% ", WTERMSIG(last_exit_status));
    } else {
        snprintf(prompt, BUFFER_SIZE, "enseash %% ");
    }
    write(STDOUT_FILENO, prompt, strlen(prompt));
}
