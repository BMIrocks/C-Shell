#include "main.h"

void add_log(const char* command){
    if (command == NULL || strlen(command) == 0)
        return;

    char clean_command[PATH_MAX];
    strncpy(clean_command, command, PATH_MAX - 1);
    clean_command[PATH_MAX - 1] = '\0';
    clean_command[strcspn(clean_command, "\n")] = '\0';

    if (LOG_SIZE < MAX_LOG_COUNT) {
        LOG_BUFFER[(LOG_START + LOG_SIZE) % MAX_LOG_COUNT] = strdup(clean_command);
        LOG_SIZE++;
    } 
    else {
        free(LOG_BUFFER[LOG_START]);
        LOG_BUFFER[LOG_START] = strdup(clean_command);
        LOG_START = (LOG_START + 1) % MAX_LOG_COUNT;
    }

    FILE *fp = fopen(LOG_FILE, "a");
    if (fp == NULL) {
        perror("add_to_log: fopen");
        return;
    }
    fprintf(fp, "%d %s\n", ++LOG_COUNT_GLOBAL, clean_command);
    fclose(fp);
}

void log_print(){
    for(int i = 0; i < LOG_SIZE; i++){
        int index = (LOG_START + i) % MAX_LOG_COUNT;
        printf("%d: %s\n", LOG_COUNT_GLOBAL-LOG_SIZE+i+1, LOG_BUFFER[index]);
    }
    return;
}

void delete_log(){
    LOG_COUNT_GLOBAL =0;
    LOG_SIZE = 0;
    LOG_START = 0;
    for(int i = 0; i < MAX_LOG_COUNT; i++){
        free(LOG_BUFFER[i]);
        LOG_BUFFER[i] = NULL;
    }
    FILE *fp = fopen(LOG_FILE, "w");
    if(fp) 
        fclose(fp);
    else
        perror("delete_log: fopen");

    return;
}

char* get_command(int index){
    if(index < 1 || index > LOG_COUNT_GLOBAL)
        return NULL;
    
    FILE* fp = fopen(LOG_FILE, "r+");
    if(fp == NULL){
        perror("get_command: fopen");
        return NULL;
    }
    char line[PATH_MAX + 50];
    int i;
    char command[PATH_MAX];

    while(fgets(line, sizeof(line), fp) != NULL){
        line[strcspn(line, "\n")] = '\0';
        if(sscanf(line, "%d %[^\n]", &i, command) == 2){
            if(i == index){
                fclose(fp);
                return strdup(command);
            }
        }
    }

    fclose(fp);
    return NULL;
}