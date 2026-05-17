#include "main.h"


void reveal(const char *path, int a_flag, int l_flag){

    char resolved_path[PATH_MAX];
    if (path == NULL || strlen(path) == 0) {
        strcpy(resolved_path, ".");
    }
    else if (path[0] == '~') {
        if (path[1] == '\0') 
            strcpy(resolved_path, HOME);
        else if (path[1] == '/') 
            snprintf(resolved_path, sizeof(resolved_path), "%s%s", HOME, path + 1);
        else{
            fprintf(stderr, "Unsupported ~ usage: %s\n", path);
            return;
        }
    }
    else 
        strncpy(resolved_path, path, sizeof(resolved_path));


    DIR* dir = opendir(resolved_path);
    if (dir == NULL) {
        perror("reveal: opendir failed");
        return;
    }

    struct dirent *entry;
    char *entries[1024];
    int count = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (!a_flag && entry->d_name[0] == '.')
            continue;

        entries[count] = strdup(entry->d_name);  
        if (entries[count] == NULL) {
            perror("reveal: strdup failed");
            closedir(dir);
            return;
        }
        count++;
        if (count >= 1024) break;
    }
    closedir(dir);

    qsort(entries, count, sizeof(char *), cmpstring);

    for (int i = 0; i < count; ++i) {
        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", resolved_path, entries[i]);

        struct stat st;
        if (stat(full_path, &st) == -1) {
            perror("reveal: stat failed");
            printf("%s\n", entries[i]);
            free(entries[i]);
            continue;
        }
        
        if (l_flag) {
            char perms[11] = "----------";
            if (S_ISDIR(st.st_mode)) perms[0] = 'd';
            if (st.st_mode & S_IRUSR) perms[1] = 'r';
            if (st.st_mode & S_IWUSR) perms[2] = 'w';
            if (st.st_mode & S_IXUSR) perms[3] = 'x';
            if (st.st_mode & S_IRGRP) perms[4] = 'r';
            if (st.st_mode & S_IWGRP) perms[5] = 'w';
            if (st.st_mode & S_IXGRP) perms[6] = 'x';
            if (st.st_mode & S_IROTH) perms[7] = 'r';
            if (st.st_mode & S_IWOTH) perms[8] = 'w';
            if (st.st_mode & S_IXOTH) perms[9] = 'x';

            struct passwd *pw = getpwuid(st.st_uid);
            struct group  *gr = getgrgid(st.st_gid);
            char timebuf[64];
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&st.st_mtime));

            printf("%s %2hu %s %s %5lld %s ", perms, st.st_nlink,
                pw ? pw->pw_name : "?", gr ? gr->gr_name : "?",
                (long long)st.st_size, timebuf);
        }

        if (S_ISDIR(st.st_mode))
            printf(COLOR_BLUE "%s\n" COLOR_RESET, entries[i]);
        else if (st.st_mode & S_IXUSR)
            printf(COLOR_GREEN "%s\n" COLOR_RESET, entries[i]);
        else
            printf(COLOR_BLACK "%s\n" COLOR_RESET, entries[i]);
        
        free(entries[i]);
    }

    return;
}

int cmpstring(const void *a, const void *b) {
    const char **sa = (const char **)a;
    const char **sb = (const char **)b;
    return strcmp(*sa, *sb);
}

