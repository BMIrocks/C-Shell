#include "main.h"

static int copy_path(char *dest, size_t dest_size, const char *src) {
    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        return -1;
    }

    memcpy(dest, src, src_len + 1);
    return 0;
}

void hop(char* path){
    char target_dir[PATH_MAX];
    char current_dir[PATH_MAX];

    if(path == NULL || strlen(path) == 0){ 
        if (copy_path(target_dir, sizeof(target_dir), HOME) != 0) {
            fprintf(stderr, "hop: home path too long\n");
            return;
        }
    }
    else if(strcmp(path,"-") == 0){
        if (copy_path(target_dir, sizeof(target_dir), PREVIOUS_DIR) != 0) {
            fprintf(stderr, "hop: previous directory path too long\n");
            return;
        }
    }
    else if(strncmp(path,"~",1)==0){
        if(strlen(path) == 1){
            if (copy_path(target_dir, sizeof(target_dir), HOME) != 0) {
                fprintf(stderr, "hop: home path too long\n");
                return;
            }
        }
        else if(path[1]=='/'){
            int written = snprintf(target_dir, sizeof(target_dir), "%s%s", HOME, path+1);
            if (written < 0 || (size_t)written >= sizeof(target_dir)) {
                fprintf(stderr, "hop: target path too long\n");
                return;
            }
        }
        else{
            fprintf(stderr, "Unsupported use of ~: %s\n", path);
            return;
        }
    }
    else{
        if (copy_path(target_dir, sizeof(target_dir), path) != 0) {
            fprintf(stderr, "hop: target path too long\n");
            return;
        }
    }

    if(getcwd(current_dir,sizeof(current_dir)) == 0){
        perror("getcwd() error");
        return;
    }
    if(chdir(target_dir) == -1){
        perror("chdir() error");
        return;
    }
    if (copy_path(PREVIOUS_DIR, sizeof(PREVIOUS_DIR), current_dir) != 0) {
        fprintf(stderr, "hop: current directory path too long\n");
        return;
    }
    if(getcwd(current_dir, sizeof(current_dir)) == NULL){
        perror("getcwd() error");
        return;
    }
    printf("Changed directory to: %s\n", current_dir);
    return;
}
