#include "main.h"
#include "echo.h"

void echo_command(char **args, int arg_count) {
    int newline = 1; 
    int start_index = 0; 
    
    if (arg_count > 0 && args[0] != NULL && strcmp(args[0], "-n") == 0) {
        newline = 0;
        start_index = 1; 
    }
    
    for (int i = start_index; i < arg_count; i++) {
        if (args[i] != NULL) {
            
            if (i > start_index) {
                printf(" ");
            }
            printf("%s", args[i]);
        }
    }
    
    if (newline) {
        printf("\n");
    }
}
