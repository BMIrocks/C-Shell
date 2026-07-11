#include "main.h"

char HOME[PATH_MAX];
char USERNAME[256];
char HOSTNAME[256];
char PREVIOUS_DIR[PATH_MAX];
char SHELL_ROOT[PATH_MAX];
char LOG_FILE_PATH[PATH_MAX];

char* LOG_BUFFER[MAX_LOG_COUNT];   
int LOG_START = 0;                     
int LOG_SIZE = 0;                      
int LOG_COUNT_GLOBAL = 0;

static int copy_string(char *dest, size_t dest_size, const char *src) {
    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        return -1;
    }

    memcpy(dest, src, src_len + 1);
    return 0;
}

void initialise_global(){

    
    // sets the shell home directory to the user's actual home directory
    char *home_env = getenv("HOME");
    if (home_env != NULL && copy_string(HOME, sizeof(HOME), home_env) == 0) {
        // HOME from environment was used successfully.
    }
    else {
        struct passwd *pw = getpwuid(getuid());
        if (pw == NULL || copy_string(HOME, sizeof(HOME), pw->pw_dir) != 0) {
            fprintf(stderr, "Unable to determine home directory\n");
            exit(EXIT_FAILURE);
        }
    }
    
    // keeps the launch directory for project-local state such as history
    if (getcwd(SHELL_ROOT, sizeof(SHELL_ROOT)) == NULL) {
        perror("getcwd() error");
        exit(EXIT_FAILURE);
    }

    if (copy_string(PREVIOUS_DIR, sizeof(PREVIOUS_DIR), SHELL_ROOT) != 0) {
        fprintf(stderr, "Initial directory path is too long\n");
        exit(EXIT_FAILURE);
    }

    int written = snprintf(LOG_FILE_PATH, sizeof(LOG_FILE_PATH), "%s/%s", SHELL_ROOT, LOG_FILE);
    if (written < 0 || (size_t)written >= sizeof(LOG_FILE_PATH)) {
        fprintf(stderr, "Log file path is too long\n");
        exit(EXIT_FAILURE);
    }

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
    // fopen with "a+" mode creates the history file if it does not exist.

    FILE *log_file = fopen(LOG_FILE_PATH, "a+");
    if(!log_file){
        perror("Error opening log file");
        return;
    }

    rewind(log_file);

    char line[PATH_MAX + 50];
    // goes through each entry and updates the log buffer,log size and log start
    while(fgets(line, sizeof(line), log_file) != NULL){
        line[strcspn(line, "\n")] = '\0'; 
        
        int index;
        char command_buffer[PATH_MAX];
        if(sscanf(line, "%d %[^\n]", &index, command_buffer) == 2){
            LOG_COUNT_GLOBAL = index;
            char *copy = strdup(command_buffer);
            if (copy == NULL) {
                perror("strdup");
                break;
            }

            LOG_BUFFER[(LOG_START + LOG_SIZE) % MAX_LOG_COUNT] = copy;

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

void cleanup_global() {
    for (int i = 0; i < MAX_LOG_COUNT; ++i) {
        free(LOG_BUFFER[i]);
        LOG_BUFFER[i] = NULL;
    }
    LOG_START = 0;
    LOG_SIZE = 0;
}
