#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

bool includes(const char* string, const char* arr[], int size);

int main() {
    bool canRepeat = true;
    const char* built_in_commands[] = {"exit", "type", "echo" , "pwd" , "cd"};
    int num_commands = sizeof(built_in_commands) / sizeof(built_in_commands[0]);
    
    while (canRepeat) {
        printf("$ ");
        fflush(stdout);
        
        char input[200];
        if (!fgets(input, sizeof(input), stdin)) {
            break; // Handle EOF or input error
        }
        
        // Remove the trailing newline
        input[strlen(input) - 1] = '\0';
        
        // "exit" command
        if (!strcmp(input, "exit 0")) {
            exit(0);
        }
        
        // "echo" command
        else if (strncmp(input, "echo", strlen("echo")) == 0) {
            printf("%s\n", input + strlen("echo") + 1);
        }
        
        else if (strcmp(input, "pwd") == 0) {
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
        }
        

        else if (strncmp(input, "cd", strlen("cd")) == 0) {
            char* path = strtok(input, " ");
            path = strtok(NULL, " ");

            if (path == NULL || strcmp(path, "~") == 0) {
             
                char* home_dir = getenv("HOME");
                if (home_dir == NULL) {
                    fprintf(stderr, "cd: HOME environment variable is not set\n");
                } else if (chdir(home_dir) < 0) {
                    perror("cd");
                }
            } else {
          
                if (chdir(path) < 0) {
                    fprintf(stderr, "cd: %s: No such file or directory\n", path);
                }
            }
        }
        // "type" command
        else if (strncmp(input, "type", strlen("type")) == 0) {
            char* command = strtok(input, " ");
            char* argument = strtok(NULL, " ");
            bool found = false;
            
            if (includes(argument, built_in_commands, num_commands)) {
                printf("%s is a shell builtin\n", argument);
                continue;
            } else {
                char* path = getenv("PATH");
                if (path == NULL) {
                    printf("PATH not set\n");
                    continue;
                }
                
                char* path_copy = strdup(path);
                if (!path_copy) {
                    perror("strdup");
                    exit(EXIT_FAILURE);
                }
                
                char* dir = strtok(path_copy, ":");
                while (dir != NULL) {
                    char full_path[1024];
                    snprintf(full_path, sizeof(full_path), "%s/%s", dir, argument);
                    
                    if (access(full_path, X_OK) == 0) {
                        printf("%s is %s\n", argument, full_path);
                        found = true;
                        break;
                    }
                    
                    dir = strtok(NULL, ":");
                }
                
                free(path_copy);
            }
            
            if (!found) {
                printf("%s: not found\n", argument);
            }
        }
        
        // Handle external commands
        else {
            printf("%s \n" , "executing");
            pid_t pid = fork(); // create child process
            
            if (pid == 0) {
                // Child process
                char* args[200]; // array to store command and arguments
                int i = 0;
                
                // Tokenize the input string
                char* token = strtok(input, " ");
                
                while (token != NULL) {
                    args[i++] = token;
                    token = strtok(NULL, " ");
                }
                args[i] = NULL; // Null-terminate the argument list
                
                // Execute the command
                execvp(args[0], args);
                
                
                fprintf(stderr, "%s: command not found\n", args[0]);
                exit(EXIT_FAILURE); // Exit with failure code
            } else if (pid > 0) {
                // Parent process waits for the child process
                int status;
                wait(&status);
            } else {
                // fork() failed
                perror("fork");
            }
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

