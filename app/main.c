#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

int main() {
    // Uncomment this block to pass the first stage
    bool canRepeat = true;
    char* built_in_commands[] = {"exit" , "type" , "echo"};
    
    // Calculate the number of elements in the array
    int num_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);
    
    //      // Loop through the array
    //      for (int x = 0; x < num_commands; x++) {
    //          printf("Command %i: %s\n", x, built_in_commands[x]);
    //      }
    //
    //
    //    return 0;
    while (canRepeat) {
        printf("$ ");
        fflush(stdout);
        
        // Wait for user input
        char input[100];
        fgets(input, 100, stdin);
        
        input[strlen(input) - 1] = '\0';
        
        if(!strcmp(input , "exit 0")){
            exit(0);
        }
        
        if (strncmp(input, "echo", strlen("echo")) == 0) {
            printf("%s\n", input + strlen("echo") + 1);
            continue;
        }
        
        
        char* command = strtok(input, " ");
        char* argument = strtok(NULL, " ");
        
        
        if (strcmp(command , "type") == 0) {
            for (int x = 0; x < num_commands; x++) {
                
                if (strcmp(argument, built_in_commands[x]) == 0) {
                    printf("%s is a shell builtin \n" , argument);
                    continue;
                    
                }
            }
         
        }
        
        
        printf("%s: command not found\n", input);
        
    }
    return 0;
}
