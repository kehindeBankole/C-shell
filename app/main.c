#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    bool canRepeat = true;

    while (canRepeat) {
        printf("$ ");
        fflush(stdout);
        
        // Wait for user input
        char input[200];
        if (!fgets(input, sizeof(input), stdin)) {
            break;  // Exit on EOF
        }

        // Remove the trailing newline
        input[strcspn(input, "\n")] = '\0';

        // "exit" command
        if (strcmp(input, "exit") == 0) {
            exit(0);
        }
        // "echo" command
        else if (strncmp(input, "echo", strlen("echo")) == 0) {
            printf("%s\n", input + strlen("echo") + 1);
        }
        // "type" command
        else if (strncmp(input, "type", strlen("type")) == 0) {
            char* command = strtok(input, " ");
            char* argument = strtok(NULL, " ");

            if (!argument) {
                printf("type: missing argument\n");
                continue;
            }

            char* path = getenv("PATH");
            if (!path) {
                printf("PATH not set\n");
                continue;
            }

            // Make a copy of PATH to avoid modifying the original
            char* path_copy = strdup(path);
            if (!path_copy) {
                perror("strdup");
                exit(EXIT_FAILURE);
            }

            char* dir = strtok(path_copy, ":");
            bool found = false;

            while (dir) {
                char full_path[1024];
                snprintf(full_path, sizeof(full_path), "%s/%s", dir, argument);

                if (access(full_path, X_OK) == 0) {
                    printf("%s is %s\n", argument, full_path);
                    found = true;
                    break;
                }

                dir = strtok(NULL, ":");
            }

            if (!found) {
                printf("%s: not found\n", argument);
            }

            free(path_copy);  // Free the duplicated PATH
        }
        // Unknown command
        else {
            printf("%s: command not found\n", input);
        }
    }

    return 0;
}

