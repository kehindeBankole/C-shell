#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

int main() {
  // Uncomment this block to pass the first stage
    bool canRepeat = true;
    
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
        
         printf("%s: command not found\n", input);

    }
  return 0;
}
