#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
bool includes(const char* string, const char* arr[], int size);

int main() {
    bool canRepeat = true;
    const char* built_in_commands[] = {"exit", "type", "echo"};
    
    // Calculate the number of elements in the array
    int num_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);
    
    while (canRepeat) {
        printf("$ ");
        fflush(stdout);
        
        // Wait for user input
        char input[200];  // Increased size to handle longer inputs
        fgets(input, 200, stdin);
        
        // Remove the trailing newline
        input[strlen(input) - 1] = '\0';
        
        // "exit" command
        if (!strcmp(input, "exit")) {
            exit(0);
        }
        //  "echo" command
        else if (strncmp(input, "echo", strlen("echo")) == 0) {
            // Skip over "echo" and the space
            printf("%s\n", input + strlen("echo") + 1);  // +1 to skip the space
        }
        //"type" command
        else if (strncmp(input, "type", strlen("type")) == 0) {
            // Split the input into command and argument
            char* command = strtok(input, " ");
            char* argument = strtok(NULL, " ");
            bool found = false;
            
            
            if(includes(argument , built_in_commands , num_commands)){
                
                for (int x = 0; x < num_commands; x++) {
                    
                    if (strcmp(argument, built_in_commands[x]) == 0) {
                        found = true;
                        printf("%s is a shell builtin\n" , argument);
                        
                        continue;
                    }
                }
                
                continue;
                
            }else{
                
                
                // Search for the command in the directories listed in PATH
                char* path = getenv("PATH");
                if (path == NULL) {
                    printf("PATH not set\n");
                    continue;
                }
                
                // copy path
                char* path_copy = strdup(path);
                if (!path_copy) {
                    perror("strdup");
                    exit(0);
                }
                
                // Tokenize the PATH into individual directories
                char* dir = strtok(path_copy, ":");
                
                
                
                
                while (dir != NULL) {
       
                    char full_path[1024];
                    snprintf(full_path, sizeof(full_path), "%s/%s", dir, argument);
                    
                    //  file exists and is executable
                    if (access(full_path, X_OK) == 0) {
                        printf("%s is %s\n", argument, full_path);
                        found = true;
                        break;
                    }
                    
                    dir = strtok(NULL, ":");
                }
                
                free(path_copy);
                
            }
            
            
            
            // If the command was not found
            if (!found) {
                printf("%s: not found\n", argument);
            }
            
            
        }
        
        
        else {
            printf("%s: command not found\n", input);
        }
    }
    
    return 0;
}



bool includes(const char* string, const char* arr[], int size) {
    for (int x = 0; x < size; x++) {
        if (strcmp(string, arr[x]) == 0) {
            return true; // Found
        }
    }
    return false; // Not found
}
