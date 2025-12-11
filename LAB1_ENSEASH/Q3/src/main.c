#include "main.h"


int main(int argc, char *argv[]) {
    char buffer[BUFFER_SIZE]; // buffer memory to store incoming user's instruction(s) max BUFFER_SIZE times bytes
    ssize_t bytes_read;       // single byte read (8 bits)

    /*
    * writes shellWelcomeMsg an uint8_t pointer of size strlen(shellWelcomeMsg) to file descriptor
    * STDOUT_FILENO : stand for POSIX's standard output screen
    */ 
    write(STDOUT_FILENO, shellWelcomeMsg, strlen(shellWelcomeMsg)); 

    
    while (1) {
        // Basic display prompt of 'shellName'
        write(STDOUT_FILENO, shellName, strlen(shellName));

        /*
        * Reads incoming user's information(s) as uint8_t pointer of max size BUFFER_SIZE
        * Blocks until it receives user's data
        * STDIN_FILENO : stand for POSIX's standard input keyboard
        */ 
        bytes_read = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
        
        // Ctrl + d return EOF signal (meaning 0), (also prevent null sized incoming data)
        if (bytes_read <= 0){
            doExitCmdMethod();
            break;
        } 

        if (handleCommand(buffer) == 1 ) {
            break; // Exit the loop if handleCommand returns 1
        }
    }

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
    } else if (strcmp(buffer, DATECMD) == 0) {
        doDateCmdMethod();
    } else if (strcmp(buffer, EXITCMD) == 0) {
        doExitCmdMethod();
        return 1; // Signal to exit
    } else {
        write(STDOUT_FILENO, "Unknown command\n", 15);
    }
    return 0; // Continue
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
* Check on user request exit
*/
void doExitCmdMethod(void){
    write(STDOUT_FILENO, "Bye bye...\n", 11);
}