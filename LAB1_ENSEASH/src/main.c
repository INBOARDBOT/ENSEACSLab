#include "main.h"

// const char* (string) magic shell catchphrase 
const char* shellName = "enseash % ";
const char* shellWelcomeMsg = "Welcome to ENSEA magic Shell.\nType 'exit' to quit.\n";


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
        
        /*
        * Check on user request exit
        * strncmp() function from string lib compare 4 first bytes of buffer char pointer
        * with given reference "exit".
        * 3 cases are possible either each string compared are equal or inferior or superior 
        * to one another, if they are equal it returns 0 (what we want to verify).
        */
        if (strncmp(buffer, "exit", 4) == 0) {
            write(STDOUT_FILENO, "Bye bye...\n", 11);
            break;
        }

    }

    // is returned when everything is going wonderfully well
    return EXIT_SUCCESS;
}
