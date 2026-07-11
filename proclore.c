#include "main.h"
#include "proclore.h"
#include <errno.h>

#ifdef __APPLE__
#include <sys/sysctl.h>
#include <libproc.h>
#endif

static int linux_process_info(pid_t pid, char *process_status, size_t process_status_size) {
#ifndef __APPLE__
    char stat_path[64];
    FILE *stat_file;
    char line[1024];
    int parsed_pid;
    char comm[256];
    char state;
    int parent_pid;
    int process_group;

    snprintf(stat_path, sizeof(stat_path), "/proc/%d/stat", pid);
    stat_file = fopen(stat_path, "r");
    if (stat_file == NULL) {
        return -1;
    }

    if (fgets(line, sizeof(line), stat_file) == NULL) {
        fclose(stat_file);
        return -1;
    }

    fclose(stat_file);

    if (sscanf(line, "%d (%255[^)]) %c %d %d", &parsed_pid, comm, &state, &parent_pid, &process_group) != 5) {
        return -1;
    }

    (void)parsed_pid;
    (void)comm;
    (void)parent_pid;

    switch (state) {
        case 'R':
            strncpy(process_status, "R", process_status_size);
            break;
        case 'S':
        case 'D':
            strncpy(process_status, "S", process_status_size);
            break;
        case 'T':
        case 't':
            strncpy(process_status, "T", process_status_size);
            break;
        case 'Z':
            strncpy(process_status, "Z", process_status_size);
            break;
        default:
            strncpy(process_status, "S", process_status_size);
            break;
    }

    process_status[process_status_size - 1] = '\0';

    pid_t terminal_pgrp = tcgetpgrp(STDIN_FILENO);
    if (terminal_pgrp != -1 && process_group == terminal_pgrp && state != 'Z') {
        size_t status_length = strlen(process_status);
        if (status_length + 1 < process_status_size) {
            strcat(process_status, "+");
        }
    }

    return process_group;
#else
    (void)pid;
    (void)process_status;
    (void)process_status_size;
    return -1;
#endif
}


static void linux_print_virtual_memory(pid_t pid) {
#ifndef __APPLE__
    char status_path[64];
    FILE *status_file;
    char line[1024];

    snprintf(status_path, sizeof(status_path), "/proc/%d/status", pid);
    status_file = fopen(status_path, "r");
    if (status_file == NULL) {
        printf("Virtual memory : [could not access]\n");
        return;
    }

    while (fgets(line, sizeof(line), status_file) != NULL) {
        if (strncmp(line, "VmSize:", 7) == 0) {
            unsigned long vm_kb = 0;
            if (sscanf(line + 7, "%lu", &vm_kb) == 1) {
                printf("Virtual memory : %lu\n", vm_kb * 1024UL);
            } else {
                printf("Virtual memory : [could not access]\n");
            }

            fclose(status_file);
            return;
        }
    }

    fclose(status_file);
    printf("Virtual memory : [could not access]\n");
#endif
}

static void linux_print_executable_path(pid_t pid) {
#ifndef __APPLE__
    char exe_link[64];
    char exe_path[PATH_MAX];
    ssize_t path_length;

    snprintf(exe_link, sizeof(exe_link), "/proc/%d/exe", pid);
    path_length = readlink(exe_link, exe_path, sizeof(exe_path) - 1);
    if (path_length >= 0) {
        exe_path[path_length] = '\0';
        printf("executable Path : %s\n", exe_path);
    } else {
        printf("executable Path : [could not access]\n");
    }
#endif
}

void proclore(char *pid_str) {
    pid_t pid;
    
    if (pid_str == NULL) {
        pid = getpid();
    } 
    else {
        char *endptr;
        long temp_pid = strtol(pid_str, &endptr, 10);
        
        if (*endptr != '\0' || temp_pid <= 0) {
            printf("Invalid PID: %s\n", pid_str);
            return;
        }
        pid = (pid_t)temp_pid;
    }
    
    // Check if process exists using kill with signal 0
    if (kill(pid, 0) == -1 && errno == ESRCH) {
        printf("No such process found\n");
        return;
    }
    
    printf("pid : %d\n", pid);

#ifdef __APPLE__
    // Get process information using sysctl
    struct kinfo_proc proc_info;
    size_t size = sizeof(proc_info);
    int mib[4] = {CTL_KERN, KERN_PROC, KERN_PROC_PID, pid};

    if (sysctl(mib, 4, &proc_info, &size, NULL, 0) == -1) {
        printf("Error getting process information\n");
        return;
    }

    // Get process status
    char process_status[4];
    switch(proc_info.kp_proc.p_stat) {
        case SRUN:
            strcpy(process_status, "R");
            break;
        case SSLEEP:
            strcpy(process_status, "S");
            break;
        case SSTOP:
            strcpy(process_status, "T");
            break;
        case SZOMB:
            strcpy(process_status, "Z");
            break;
        default:
            strcpy(process_status, "S");
            break;
    }

    // Check if it's a foreground process
    pid_t terminal_pgrp = tcgetpgrp(STDIN_FILENO);
    pid_t process_pgrp = proc_info.kp_eproc.e_pgid;

    if (process_pgrp == terminal_pgrp && proc_info.kp_proc.p_stat != SZOMB) {
        strcat(process_status, "+");
    }

    printf("process Status : %s\n", process_status);
    printf("Process Group : %d\n", process_pgrp);

    // Get virtual memory size
    struct proc_taskinfo task_info;
    if (proc_pidinfo(pid, PROC_PIDTASKINFO, 0, &task_info, sizeof(task_info)) > 0) {
        printf("Virtual memory : %llu\n", task_info.pti_virtual_size);
    }
    else {
        printf("Virtual memory : [could not access]\n");
    }

    // Get executable path
    char exe_path[PROC_PIDPATHINFO_MAXSIZE];
    if (proc_pidpath(pid, exe_path, sizeof(exe_path)) > 0) {
        printf("executable Path : %s\n", exe_path);
    }
    else {
        printf("executable Path : [could not access]\n");
    }
#else
    char process_status[4] = "S";
    int process_pgrp = linux_process_info(pid, process_status, sizeof(process_status));

    if (process_pgrp < 0) {
        printf("Error getting process information\n");
        return;
    }

    printf("process Status : %s\n", process_status);
    printf("Process Group : %d\n", process_pgrp);

    linux_print_virtual_memory(pid);
    linux_print_executable_path(pid);
#endif
}