#include "main.h"

void print_prompt() {
    char cwd[PATH_MAX];   
    char prompt[PATH_MAX + 100]; 
    
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        perror("getcwd() error");
        return;
    }

    if(strncmp(cwd,HOME,strlen(HOME)) == 0) {
        if(strcmp(cwd,HOME) == 0) {
            snprintf(prompt, sizeof(prompt), "%s@%s:~$ ",USERNAME,HOSTNAME);
        } 
        else {
            snprintf(prompt, sizeof(prompt), "%s@%s:~%s$ ",USERNAME, HOSTNAME, cwd + strlen(HOME));
        }
    } 
    else {
        snprintf(prompt, sizeof(prompt), "%s@%s:%s$ ", USERNAME, HOSTNAME, cwd);
    }
    printf("%s",prompt);
    return;
}