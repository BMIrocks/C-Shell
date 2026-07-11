#include "main.h"
#include "globals.h"
#include "prompt.h"
#include "hop.h"
#include "reveal.h"
#include "log.h"
#include "proclore.h"
#include "seek.h"
#include "echo.h"
#include "iman.h"

#define MAX_COMMANDS 128
#define MAX_ARGS 128

static int split_commands(char *input, char **commands, int max_commands) {
    int count = 0;
    int in_single_quotes = 0;
    int in_double_quotes = 0;
    char *command_start = input;

    for (char *cursor = input; ; ++cursor) {
        char current = *cursor;

        if (current == '\0') {
            while (isspace((unsigned char)*command_start)) {
                command_start++;
            }
            if (*command_start != '\0' && count < max_commands) {
                commands[count++] = command_start;
            }
            break;
        }

        if (current == '\'' && !in_double_quotes) {
            in_single_quotes = !in_single_quotes;
            continue;
        }

        if (current == '"' && !in_single_quotes) {
            in_double_quotes = !in_double_quotes;
            continue;
        }

        if (current == ';' && !in_single_quotes && !in_double_quotes) {
            *cursor = '\0';
            while (isspace((unsigned char)*command_start)) {
                command_start++;
            }
            if (*command_start != '\0' && count < max_commands) {
                commands[count++] = command_start;
            }
            command_start = cursor + 1;
        }
    }

    return count;
}

static int parse_arguments(char *line, char **args, int max_args) {
    int count = 0;
    char *read_cursor = line;
    char *write_cursor = line;

    while (*read_cursor != '\0') {
        while (isspace((unsigned char)*read_cursor)) {
            read_cursor++;
        }

        if (*read_cursor == '\0') {
            break;
        }

        if (count >= max_args) {
            fprintf(stderr, "Too many arguments\n");
            return -1;
        }

        args[count++] = write_cursor;
        char quote = '\0';

        while (*read_cursor != '\0') {
            if (quote == '\0' && isspace((unsigned char)*read_cursor)) {
                break;
            }

            if ((*read_cursor == '\'' || *read_cursor == '"')) {
                if (quote == '\0') {
                    quote = *read_cursor++;
                    continue;
                }
                if (*read_cursor == quote) {
                    quote = '\0';
                    read_cursor++;
                    continue;
                }
            }

            if (*read_cursor == '\\' && read_cursor[1] != '\0') {
                read_cursor++;
            }

            *write_cursor++ = *read_cursor++;
        }

        if (quote != '\0') {
            fprintf(stderr, "Unterminated quote in command\n");
            return -1;
        }

        if (*read_cursor != '\0') {
            read_cursor++;
        }
        *write_cursor++ = '\0';
    }

    return count;
}

int main() {
    initialise_global();
    char input[PATH_MAX];

    while (1) {
        print_prompt();

        if (fgets(input, sizeof(input), stdin) == NULL) {
            if (feof(stdin)) {
                printf("\n");
                break;
            }
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

        char *commands[MAX_COMMANDS];
        int command_count = split_commands(input, commands, MAX_COMMANDS);

        for (int command_index = 0; command_index < command_count; ++command_index) {
            char *args[MAX_ARGS];
            int arg_count = parse_arguments(commands[command_index], args, MAX_ARGS);
            if (arg_count <= 0) {
                continue;
            }

            char *command = args[0];

            if (strcmp(command, "hop") == 0){ 
                if (arg_count > 2) {
                    fprintf(stderr, "hop: too many arguments\n");
                    continue;
                }
                hop(arg_count > 1 ? args[1] : NULL);
            }
            else if(strcmp(command, "reveal") == 0){
                int a_flag = 0;
                int l_flag = 0;
                char *path = NULL;
                
                for (int i = 1; i < arg_count; ++i) {
                    if(args[i][0] == '-'){
                        for(int j = 1; args[i][j] != '\0'; j++){
                            if(args[i][j] == 'a')
                                a_flag = 1;
                            else if(args[i][j] == 'l')
                                l_flag = 1;
                            else{
                                fprintf(stderr, "reveal: invalid flag '%c'\n", args[i][j]);
                            }
                        }
                    }
                    else{
                        if(path == NULL) {
                            path = args[i];
                        } else {
                            fprintf(stderr, "reveal: too many paths provided\n");
                            path = NULL;
                            break;
                        }
                    }
                }
                if (path == NULL && arg_count > 1) {
                    if (a_flag || l_flag) {
                        reveal(NULL, a_flag, l_flag);
                    }
                    continue;
                }
                reveal(path, a_flag, l_flag);
            }
            else if(strcmp(command, "log") == 0){
                if(arg_count > 1){
                    if(args[1][0] == '-'){
                        if(strcmp(args[1], "-d") == 0){
                            delete_log();
                            printf("Log deleted\n");
                        }
                        else
                            fprintf(stderr, "log: invalid flag '%s'\n", args[1]);
                    }
                }
                else
                    log_print();
            }
            else if(strcmp(command, "proclore") == 0){
                if (arg_count > 2) {
                    fprintf(stderr, "proclore: too many arguments\n");
                    continue;
                }
                proclore(arg_count > 1 ? args[1] : NULL);
            }
            else if(strcmp(command, "seek") == 0){
                seek(args + 1, arg_count - 1);
            }
            else if(strcmp(command, "echo") == 0){
                echo_command(args + 1, arg_count - 1);
            }
            else if(strcmp(command, "iMan") == 0){
                iman_command(args + 1, arg_count - 1);
            }
            else if (strcmp(command, "exit") == 0) {
                cleanup_global();
                return 0;
            }
            else {
                fprintf(stderr, "Unknown command: %s\n", command);
            }
        }
    }

    cleanup_global();
    return 0;
}
