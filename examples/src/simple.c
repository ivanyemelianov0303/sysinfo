// 
// Sysinfo
// 
// Copyright (c) 2017 Guillaume Gomez
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include "sysinfo.h"

void print_process(CProcess process) {
    RString exe = sysinfo_process_get_executable_path(process);
    printf("process[%d]: parent: %d, cpu_usage: %f, memory: %ld, executable path: '%s'\n",
           sysinfo_process_get_pid(process),
           sysinfo_process_get_parent_pid(process),
           sysinfo_process_get_cpu_usage(process),
           sysinfo_process_get_memory(process),
           exe);
    sysinfo_rstring_free(exe);
}

void check_tasks(CSystem system) {
#ifdef __linux__
    bool task_loop(pid_t pid, CProcess process, void *data) {
        (void)data;
        printf("  ");
        print_process(process);
        return true;
    }

    void *sleeping_func(void *data) {
        sleep(3);
        return data;
    }
    pthread_t thread;
    pthread_create(&thread, NULL, sleeping_func, NULL);
    sysinfo_refresh_system(system);
    CProcess process = sysinfo_get_process_by_pid(system, getpid());
    printf("\n== Task(s) for current process: ==\n");
    print_process(process);
    printf("Got %ld task(s)\n", sysinfo_process_get_tasks(process, task_loop, NULL));
#else
    (void)system;
#endif
}

bool process_loop(pid_t pid, CProcess process, void *data) {
    unsigned int *i = data;

    print_process(process);
    *i += 1;
    return *i < 10;
}

int main() {
    CSystem system = sysinfo_init();
    sysinfo_refresh_all(system);
    printf("total memory:    %ld\n", sysinfo_get_total_memory(system));
    printf("free memory:     %ld\n", sysinfo_get_free_memory(system));
    printf("used memory:     %ld\n", sysinfo_get_used_memory(system));
    printf("total swap:      %ld\n", sysinfo_get_total_swap(system));
    printf("free swap:       %ld\n", sysinfo_get_free_swap(system));
    printf("used swap:       %ld\n", sysinfo_get_used_swap(system));
    printf("network income:  %ld\n", sysinfo_get_network_income(system));
    printf("network outcome: %ld\n", sysinfo_get_network_outcome(system));
    unsigned int len = 0, i = 0;
    float *procs = NULL;
    sysinfo_get_processors_usage(system, &len, &procs);
    while (i < len) {
        printf("Processor #%d usage: %f\n", i, procs[i]);
        i += 1;
    }
    free(procs);

    // processes part
    i = 0;
    printf("For a total of %ld processes.\n", sysinfo_get_processes(system, process_loop, &i));
    check_tasks(system);
    // we can now free the CSystem object.
    sysinfo_destroy(system);
    return 0;
}
