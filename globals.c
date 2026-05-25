#include "main.h"

char HOME[PATH_MAX];
char USERNAME[256];
char HOSTNAME[256];
char PREVIOUS_DIR[PATH_MAX];

char* LOG_BUFFER[MAX_LOG_COUNT];   
int LOG_START = 0;                     
int LOG_SIZE = 0;                      
int LOG_COUNT_GLOBAL = 0;

void initialise_global(){

    
    // sets current directory as home directory
    if(getcwd(HOME, sizeof(HOME)) == NULL){
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }
    
    //sets previous directory to current directory that is home directory
    strcpy(PREVIOUS_DIR, HOME);

    // gets username of computer first using env and then using getuid
    char* username = getenv("USER");
    if(username == NULL){
        struct passwd *pw = getpwuid(getuid());
        if(pw)
            username = pw->pw_name;
        else    
            username = "unknown";
    }
    strncpy(USERNAME, username, sizeof(USERNAME)-1);
    USERNAME[sizeof(USERNAME)-1] = '\0';

    // gets hostname of computer
    char hostname[256];
    if(gethostname(hostname, sizeof(hostname)) == -1) {
        perror("gethostname() error");
        return;
    }

    char *lastdot = strrchr(hostname, '.');
    if (lastdot && (strcmp(lastdot, ".local") == 0 || strcmp(lastdot, ".lan") == 0 || strcmp(lastdot, ".home") == 0 || strcmp(lastdot, ".domain") == 0)){
        *lastdot = '\0';  // Truncate at last dot
    } // this searches for last dot and replaces it with null terminator to remove domain name such as .local/.lan etc
    strncpy(HOSTNAME, hostname, sizeof(HOSTNAME)-1);
    HOSTNAME[sizeof(HOSTNAME)-1] = '\0';


    // initialises log buffer, which is a circular array.
    // fopen with "a+" mode to append to the file and read from it, if fle does not exist it will be created

    FILE *log_file = fopen(LOG_FILE, "r+");
    if(!log_file){
        perror("Error opening log file");
        return;
    }

    char line[PATH_MAX + 50];
    // goes through each entry and updates the log buffer,log size and log start
    while(fgets(line, sizeof(line), log_file) != NULL){
        line[strcspn(line, "\n")] = '\0'; 
        
        int index;
        char command_buffer[PATH_MAX];
        if(sscanf(line, "%d %[^\n]", &index, command_buffer) == 2){
            LOG_COUNT_GLOBAL = index;
            
            LOG_BUFFER[(LOG_START + LOG_SIZE) % MAX_LOG_COUNT] = strdup(command_buffer);

            if(LOG_SIZE < MAX_LOG_COUNT){
                LOG_SIZE++;
            } else {
                LOG_START = (LOG_START + 1) % MAX_LOG_COUNT;
            }
        }
    }
    fclose(log_file);

    return;
}