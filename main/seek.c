#include "main.h"
#include "seek.h"

// Structure to store search results
typedef struct {
    char **files;
    char **directories;
    int file_count;
    int dir_count;
    int capacity;
} SearchResults;

// Initialize search results
void init_results(SearchResults *results) {
    results->capacity = 100;
    results->files = malloc(results->capacity * sizeof(char*));
    results->directories = malloc(results->capacity * sizeof(char*));
    results->file_count = 0;
    results->dir_count = 0;
}

// Free search results
void free_results(SearchResults *results) {
    for (int i = 0; i < results->file_count; i++) {
        free(results->files[i]);
    }
    for (int i = 0; i < results->dir_count; i++) {
        free(results->directories[i]);
    }
    free(results->files);
    free(results->directories);
}

// Add file to results
void add_file(SearchResults *results, const char *path) {
    if (results->file_count >= results->capacity) {
        results->capacity *= 2;
        results->files = realloc(results->files, results->capacity * sizeof(char*));
    }
    results->files[results->file_count] = strdup(path);
    results->file_count++;
}

// Add directory to results
void add_directory(SearchResults *results, const char *path) {
    if (results->dir_count >= results->capacity) {
        results->capacity *= 2;
        results->directories = realloc(results->directories, results->capacity * sizeof(char*));
    }
    results->directories[results->dir_count] = strdup(path);
    results->dir_count++;
}

// Recursive search function
void search_recursive(const char *current_path, const char *target, const char *base_path, 
                     SearchResults *results, int only_dirs, int only_files) {
    DIR *dir = opendir(current_path);
    if (dir == NULL) {
        return; // No permission or invalid directory
    }
    
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        // Skip . and ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        // Check if name matches (exact match or starts with target)
        if (strncmp(entry->d_name, target, strlen(target)) == 0) {
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", current_path, entry->d_name);
            
            struct stat st;
            if (stat(full_path, &st) == 0) {
                // Calculate relative path from base directory
                char relative_path[PATH_MAX];
                if (strcmp(current_path, base_path) == 0) {
                    strcpy(relative_path, entry->d_name);
                } else {
                    // Remove base_path prefix and add entry name
                    const char *rel_start = current_path + strlen(base_path);
                    if (*rel_start == '/') rel_start++; // Skip leading slash
                    snprintf(relative_path, sizeof(relative_path), "%s/%s", rel_start, entry->d_name);
                }
                
                if (S_ISDIR(st.st_mode)) {
                    if (!only_files) {
                        add_directory(results, relative_path);
                    }
                } else {
                    if (!only_dirs) {
                        add_file(results, relative_path);
                    }
                }
            }
        }
        
        // Recursively search subdirectories
        char subdir_path[PATH_MAX];
        snprintf(subdir_path, sizeof(subdir_path), "%s/%s", current_path, entry->d_name);
        
        struct stat st;
        if (stat(subdir_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            search_recursive(subdir_path, target, base_path, results, only_dirs, only_files);
        }
    }
    
    closedir(dir);
}

// Resolve path with ~ and . symbols
void resolve_path(const char *path, char *resolved) {
    if (path == NULL) {
        getcwd(resolved, PATH_MAX);
        return;
    }
    
    if (path[0] == '~') {
        strcpy(resolved, HOME);
        if (strlen(path) > 1) {
            strcat(resolved, path + 1);
        }
    } else if (path[0] == '/') {
        strcpy(resolved, path);
    } else {
        getcwd(resolved, PATH_MAX);
        strcat(resolved, "/");
        strcat(resolved, path);
    }
}

void seek(char **args, int arg_count) {
    int only_dirs = 0;
    int only_files = 0;
    int execute_flag = 0;
    char *target = NULL;
    char *target_dir = NULL;
    
    // Parse arguments
    for (int i = 0; i < arg_count; i++) {
        if (args[i][0] == '-') {
            // Parse flags
            for (int j = 1; args[i][j] != '\0'; j++) {
                switch (args[i][j]) {
                    case 'd':
                        only_dirs = 1;
                        break;
                    case 'f':
                        only_files = 1;
                        break;
                    case 'e':
                        execute_flag = 1;
                        break;
                    default:
                        printf("Invalid flag: %c\n", args[i][j]);
                        return;
                }
            }
        } else {
            // Non-flag arguments
            if (target == NULL) {
                target = args[i];
            } else if (target_dir == NULL) {
                target_dir = args[i];
            }
        }
    }
    
    // Check for invalid flag combination
    if (only_dirs && only_files) {
        printf("Invalid flags!\n");
        return;
    }
    
    // Check if target is provided
    if (target == NULL) {
        printf("No search target provided!\n");
        return;
    }
    
    // Resolve target directory path
    char resolved_path[PATH_MAX];
    resolve_path(target_dir, resolved_path);
    
    // Check if target directory exists and is accessible
    struct stat st;
    if (stat(resolved_path, &st) != 0 || !S_ISDIR(st.st_mode)) {
        printf("Invalid target directory!\n");
        return;
    }
    
    // Perform search
    SearchResults results;
    init_results(&results);
    
    search_recursive(resolved_path, target, resolved_path, &results, only_dirs, only_files);
    
    // Check results and handle -e flag
    int total_results = results.file_count + results.dir_count;
    
    if (total_results == 0) {
        printf("No match found!\n");
    } else if (execute_flag && total_results == 1) {
        // Handle -e flag when exactly one result is found
        if (results.file_count == 1 && results.dir_count == 0) {
            // Single file found - print its content
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", resolved_path, results.files[0]);
            
            FILE *file = fopen(full_path, "r");
            if (file == NULL) {
                printf("Missing permissions for task!\n");
            } else {
                char buffer[1024];
                while (fgets(buffer, sizeof(buffer), file) != NULL) {
                    printf("%s", buffer);
                }
                fclose(file);
            }
        } else if (results.dir_count == 1 && results.file_count == 0) {
            // Single directory found - change to it
            char full_path[PATH_MAX];
            snprintf(full_path, sizeof(full_path), "%s/%s", resolved_path, results.directories[0]);
            
            if (chdir(full_path) != 0) {
                printf("Missing permissions for task!\n");
            }
        } else {
            // Multiple results - just print them
            goto print_results;
        }
    } else {
        print_results:
        // Print all results
        for (int i = 0; i < results.dir_count; i++) {
            printf("%s%s%s\n", COLOR_BLUE, results.directories[i], COLOR_RESET);
        }
        for (int i = 0; i < results.file_count; i++) {
            printf("%s%s%s\n", COLOR_GREEN, results.files[i], COLOR_RESET);
        }
    }
    free_results(&results);
}
