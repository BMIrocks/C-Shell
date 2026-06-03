#include "main.h"

int main() {
    initialise_global();
    char input[PATH_MAX];

    while (1) {
        print_prompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            perror("fgets failed");
            continue;
        }
        
        input[strcspn(input, "\n")] = '\0';
        if (strlen(input) == 0)
            continue;

        if(input[0] == '!' && isdigit(input[1])){
            int index = atoi(input + 1);
            char* input_from_log = get_command(index);
            if(input_from_log != NULL){
                printf("Executing command: %s\n", input_from_log);
                strncpy(input, input_from_log,sizeof(input) - 1);
                input[sizeof(input) - 1] = '\0';
            }
            else{
                fprintf(stderr, "Invalid command number: %d\n", index);
                continue;
            }
            free(input_from_log); 
        }

        add_log(input);

        char *saveptr_cmds; 
        char *command_line = strtok_r(input, ";", &saveptr_cmds);

        while (command_line != NULL) {
            while (*command_line == ' ') 
                command_line++;

            char *end = command_line + strlen(command_line) - 1;
            while (end > command_line && *end == ' ')
                *end-- = '\0';

            char *saveptr_args;
            char *command = strtok_r(command_line, " ", &saveptr_args);
            char *argument = strtok_r(NULL, " ", &saveptr_args);  

            if (command == NULL) {
                command_line = strtok_r(NULL, ";", &saveptr_cmds);
                continue;
            }

            if (strcmp(command, "hop") == 0) 
                hop(argument); 
            else if(strcmp(command, "reveal") == 0){
                int a_flag = 0;
                int l_flag = 0;
                char *path = NULL;
                
                while(argument != NULL){
                    if(argument[0] == '-'){
                        for(int i = 1; argument[i] != '\0'; i++){
                            if(argument[i] == 'a')
                                a_flag = 1;
                            else if(argument[i] == 'l')
                                l_flag = 1;
                            else{
                                fprintf(stderr, "reveal: invalid flag '%c'\n", argument[i]);
                            }
                        }
                    }
                    else{
                        if(path == NULL)
                            path = argument;
                    }
                    argument = strtok_r(NULL, " ", &saveptr_args);
                }
                reveal(path, a_flag, l_flag);
            }
            else if(strcmp(command, "log") == 0){
                if(argument != NULL){
                    if(argument[0] == '-'){
                        if(argument[1] == 'd'){
                            delete_log();
                            printf("Log deleted\n");
                        }
                        else
                            fprintf(stderr, "log: invalid flag '%c'\n", argument[1]);
                    }
                }
                else
                    log_print();
            }
            else if (strcmp(command, "exit") == 0) 
                return 0;
            else 
                fprintf(stderr, "Unknown command: %s\n", command);
            
            command_line = strtok_r(NULL, ";", &saveptr_cmds);
        }
    }

    return 0;
}
