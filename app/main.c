#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>

char** parseArguments(const char* input, int* argCount);
bool includes(const char* string, const char* arr[], int size);
char* removeQuotes(const char* input);

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
            //removeQuotes(input + strlen("echo") + 1 );
            printf("%s\n", removeQuotes(input + strlen("echo") + 1 ));
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
            pid_t pid = fork(); // Create child process

            if (pid == 0) {
                // Child process
                int argCount = 0;
                char** args = parseArguments(input, &argCount);

                // Execute the command
                execvp(args[0], args);

                fprintf(stderr, "%s: command not found\n", args[0]);
                // Free allocated memory
                for (int j = 0; j < argCount; j++) {
                    free(args[j]);
                }
                free(args);
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



char* removeQuotes(const char* input) {
    int j = 0;
    int length = strlen(input);
    char* output = malloc(length + 1);

    bool in_space = false;
    bool in_quotes = false;

    for (int i = 0; i < length; i++) {
        if (input[i] == '"' || input[i] == '\'') {
          
            in_quotes = !in_quotes;
            continue;
        }

        if (input[i] == ' ' && !in_quotes) {
            if (!in_space) {
         
                output[j++] = ' ';
                in_space = true;
            }
        } else {
       
            output[j++] = input[i];
            in_space = false;
        }
    }

    output[j] = '\0';  // Null-terminate the output string
    return output;
}

char** parseArguments(const char* input, int* argCount) {
    char** args = malloc(200 * sizeof(char*)); // space for arguments
    char* temp = strdup(input); // modifiable copy of the input
    int i = 0;
    char* token = temp;
    bool in_single_quotes = false;
    bool in_double_quotes = false;

    while (*token) {
     
        while (isspace(*token)) token++;

        // Start of an argument
        char* start = token;
        while (*token) {
            if (*token == '\'' && !in_double_quotes) {
                in_single_quotes = !in_single_quotes;
            } else if (*token == '"' && !in_single_quotes) {
                in_double_quotes = !in_double_quotes;
            } else if (isspace(*token) && !in_single_quotes && !in_double_quotes) {
                break; // End of the current argument
            }
            token++;
        }

        // Extract the argument
        size_t length = token - start;
        char* arg = malloc(length + 1);
        strncpy(arg, start, length);
        arg[length] = '\0';

        // Remove surrounding quotes
        args[i++] = removeQuotes(arg);
        free(arg);

        if (!*token) break;
        token++;
    }

    args[i] = NULL;
    *argCount = i;
    free(temp);
    return args;
}
