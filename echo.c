#include "main.h"
#include "echo.h"

void echo_command(char **args, int arg_count) {
    int newline = 1; // By default, add newline at the end
    int start_index = 0; // Index to start printing arguments from
    
    // Check for -n flag (suppress trailing newline)
    if (arg_count > 0 && args[0] != NULL && strcmp(args[0], "-n") == 0) {
        newline = 0;
        start_index = 1; // Skip the -n flag when printing
    }
    
    // Print all arguments separated by spaces
    for (int i = start_index; i < arg_count; i++) {
        if (args[i] != NULL) {
            // Add space before argument if it's not the first one
            if (i > start_index) {
                printf(" ");
            }
            printf("%s", args[i]);
        }
    }
    
    // Add newline at the end unless -n flag was specified
    if (newline) {
        printf("\n");
    }
}
