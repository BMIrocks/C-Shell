#include "main.h"

static int copy_path(char *dest, size_t dest_size, const char *src) {
    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        return -1;
    }

    memcpy(dest, src, src_len + 1);
    return 0;
}

static int join_path(char *dest, size_t dest_size, const char *base, const char *name) {
    int written = snprintf(dest, dest_size, "%s/%s", base, name);
    if (written < 0 || (size_t)written >= dest_size) {
        return -1;
    }

    return 0;
}

static void free_entries(char **entries, int count) {
    for (int i = 0; i < count; ++i) {
        free(entries[i]);
    }
}

void reveal(const char *path, int a_flag, int l_flag){

    char resolved_path[PATH_MAX];
    if (path == NULL || strlen(path) == 0) {
        if (copy_path(resolved_path, sizeof(resolved_path), ".") != 0) {
            fprintf(stderr, "reveal: path too long\n");
            return;
        }
    }
    else if (path[0] == '~') {
        if (path[1] == '\0') {
            if (copy_path(resolved_path, sizeof(resolved_path), HOME) != 0) {
                fprintf(stderr, "reveal: path too long\n");
                return;
            }
        }
        else if (path[1] == '/') {
            int written = snprintf(resolved_path, sizeof(resolved_path), "%s%s", HOME, path + 1);
            if (written < 0 || (size_t)written >= sizeof(resolved_path)) {
                fprintf(stderr, "reveal: path too long\n");
                return;
            }
        }
        else{
            fprintf(stderr, "Unsupported ~ usage: %s\n", path);
            return;
        }
    }
    else if (copy_path(resolved_path, sizeof(resolved_path), path) != 0) {
        fprintf(stderr, "reveal: path too long\n");
        return;
    }


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
            free_entries(entries, count);
            return;
        }
        count++;
        if (count >= 1024) break;
    }
    closedir(dir);

    qsort(entries, count, sizeof(char *), cmpstring);

    for (int i = 0; i < count; ++i) {
        char full_path[PATH_MAX];
        if (join_path(full_path, sizeof(full_path), resolved_path, entries[i]) != 0) {
            fprintf(stderr, "reveal: path too long: %s/%s\n", resolved_path, entries[i]);
            free(entries[i]);
            continue;
        }

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

            printf("%s %2lu %s %s %5lld %s ", perms, (unsigned long)st.st_nlink,
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
