#include "main.h"

void hop(char* path){
    char target_dir[PATH_MAX];
    char current_dir[PATH_MAX];

    if(path == NULL || strlen(path) == 0){ 
        strcpy(target_dir,HOME);
    }
    else if(strcmp(path,"-") == 0){
        strcpy(target_dir,PREVIOUS_DIR);
    }
    else if(strncmp(path,"~",1)==0){
        if(strlen(path) == 1){
            strcpy(target_dir,HOME);
        }
        else if(path[1]=='/'){
            snprintf(target_dir, sizeof(target_dir), "%s%s", HOME, path+1);
        }
        else{
            fprintf(stderr, "Unsupported use of ~: %s\n", path);
            return;
        }
    }
    else{
        snprintf(target_dir, sizeof(target_dir), "%s", path);
    }

    if(getcwd(current_dir,sizeof(current_dir)) == 0){
        perror("getcwd() error");
        return;
    }
    if(chdir(target_dir) == -1){
        perror("chdir() error");
        return;
    }
    strcpy(PREVIOUS_DIR,current_dir);
    if(getcwd(current_dir, sizeof(current_dir)) == NULL){
        perror("getcwd() error");
        return;
    }
    printf("Changed directory to: %s\n", current_dir);
    return;
}