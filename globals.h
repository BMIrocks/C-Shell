#ifndef GLOBALS_H
#define GLOBALS_H


#define COLOR_RESET   "\x1b[0m"
#define COLOR_BLUE    "\x1b[1;94m"
#define COLOR_GREEN   "\x1b[1;92m"
#define COLOR_BLACK   "\x1b[1;30m"

#define MAX_LOG_COUNT 15
#define LOG_FILE "/Users/dhimantbhuva/Documents/C Shell/.log.txt"

extern char HOME[PATH_MAX];
extern char USERNAME[256];
extern char HOSTNAME[256];
extern char PREVIOUS_DIR[PATH_MAX];

extern char* LOG_BUFFER[MAX_LOG_COUNT];   
extern int LOG_START;                     
extern int LOG_SIZE;                      
extern int LOG_COUNT_GLOBAL;


void initialise_global();
#endif