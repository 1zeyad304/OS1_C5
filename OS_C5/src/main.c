#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "validation.h"
#include "rr.h"
#include "sjf.h"
 
int read_processes(Process *processes, int *quantum) {
    int n;
    scanf("%d", &n);
 
    if (n <= 0 || n > 100) return -1;
 
    /* Read time quantum */
    scanf("%d", quantum);
 
    for (int i = 0; i < n; i++) {
        int pid, at, bt;
        if (scanf("%d %d %d", &pid, &at, &bt) != 3)
            return -1;
 
        processes[i].pid             = pid;
        processes[i].arrival_time    = at;
        processes[i].burst_time      = bt;
        processes[i].remaining_time  = bt;
        processes[i].completion_time = 0;
        processes[i].waiting_time    = 0;
        processes[i].turnaround_time = 0;
        processes[i].response_time   = 0;
        processes[i].first_run       = 0;
    }
    return n;
}
 
int main() {
    Process processes[100];
    int quantum = 0;
 
    int n = read_processes(processes, &quantum);
    if (n == -1) {
        printf("{\"error\": \"Invalid input\"}\n");
        return 1;
    }
 
    if (!validate_processes(processes, n)) {
        printf("{\"error\": \"Validation failed\"}\n");
        return 1;
    }
 
    if (!validate_quantum(quantum)) {
        printf("{\"error\": \"Invalid Time Quantum\"}\n");
        return 1;
    }
 
    /* Run both algorithms — they print their own JSON segments */
    rr_scheduling(processes, n, quantum);
    sjf_scheduling(processes, n);
    srtf_scheduling(processes, n);
 
    return 0;
}