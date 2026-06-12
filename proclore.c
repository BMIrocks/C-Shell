#include "main.h"
#include "proclore.h"
#include <sys/sysctl.h>
#include <libproc.h>

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
    if (kill(pid, 0) == -1) {
        printf("No such process found\n");
        return;
    }
    
    printf("pid : %d\n", pid);
    
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
}