#include "main.h"

void print_prompt() {
    char cwd[PATH_MAX];
    
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return;
    }

    if(strncmp(cwd,HOME,strlen(HOME)) == 0) {
        if(strcmp(cwd,HOME) == 0) {
            printf("%s@%s:~$ ", USERNAME, HOSTNAME);
        } 
        else {
            printf("%s@%s:~%s$ ", USERNAME, HOSTNAME, cwd + strlen(HOME));
        }
    } 
    else {
        printf("%s@%s:%s$ ", USERNAME, HOSTNAME, cwd);
    }
    return;
}
