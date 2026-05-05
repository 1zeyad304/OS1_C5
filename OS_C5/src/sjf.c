#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "sjf.h"
#include "metrics.h"
 
void sjf_scheduling(Process *processes, int n) {
    Process proc[n];
    memcpy(proc, processes, n * sizeof(Process));
 
    int gantt[1000];
    int gantt_time[1001];
    int gantt_size = 0;
 
    int current_time = 0;
    int completed    = 0;
    int visited[n];
    memset(visited, 0, sizeof(visited));
 
    gantt_time[0] = 0;
 
    while (completed < n) {
        int selected = -1;
 
        /* Pick the shortest available burst among arrived processes */
        for (int i = 0; i < n; i++) {
            if (visited[i]) continue;
            if (proc[i].arrival_time > current_time) continue;
 
            if (selected == -1) {
                selected = i;
            } else {
                if (proc[i].burst_time < proc[selected].burst_time) {
                    selected = i;
                } else if (proc[i].burst_time == proc[selected].burst_time &&
                           proc[i].arrival_time < proc[selected].arrival_time) {
                    /* Tie-break: earlier arrival wins */
                    selected = i;
                }
            }
        }
 
        /* No process available yet — advance time */
        if (selected == -1) {
            current_time++;
            continue;
        }
 
        /* First response = start of execution (non-preemptive so RT == WT) */
        if (proc[selected].first_run == 0) {
            proc[selected].response_time = current_time;
            proc[selected].first_run     = 1;
        }
 
        gantt[gantt_size]          = proc[selected].pid;
        current_time              += proc[selected].burst_time;
        gantt_time[gantt_size + 1] = current_time;
        gantt_size++;
 
        proc[selected].completion_time = current_time;
        visited[selected]              = 1;
        completed++;
    }
 
    calculate_metrics(proc, n);
 
    printf(",\"sjf\": {");
    printf("\"gantt\": [");
    for (int i = 0; i < gantt_size; i++) {
        printf("{\"pid\": %d, \"start\": %d, \"end\": %d}",
               gantt[i], gantt_time[i], gantt_time[i+1]);
        if (i < gantt_size - 1) printf(",");
    }
    printf("],");
 
    printf("\"processes\": [");
    for (int i = 0; i < n; i++) {
        printf("{\"pid\": %d, \"at\": %d, \"bt\": %d,"
               "\"wt\": %d, \"tat\": %d, \"rt\": %d}",
               proc[i].pid, proc[i].arrival_time, proc[i].burst_time,
               proc[i].waiting_time, proc[i].turnaround_time,
               proc[i].response_time);
        if (i < n - 1) printf(",");
    }
 
    float twt = 0, ttat = 0, trt = 0;
    for (int i = 0; i < n; i++) {
        twt  += proc[i].waiting_time;
        ttat += proc[i].turnaround_time;
        trt  += proc[i].response_time;
    }
    printf("],\"avg_wt\": %.2f, \"avg_tat\": %.2f, \"avg_rt\": %.2f}",
           twt/n, ttat/n, trt/n);
}

/*
   SRTF  —  Preemptive SJF
*/
void srtf_scheduling(Process *processes, int n) {
    Process proc[n];
    memcpy(proc, processes, n * sizeof(Process));

    for (int i = 0; i < n; i++) {
        proc[i].remaining_time  = proc[i].burst_time;
        proc[i].completion_time = 0;
        proc[i].waiting_time    = 0;
        proc[i].turnaround_time = 0;
        proc[i].response_time   = 0;
        proc[i].first_run       = 0;
    }

    int gantt[5000];
    int gantt_time[5001];
    int gantt_size = 0;

    int current_time = 0;
    int completed    = 0;
    int prev_pid     = -1;

    gantt_time[0] = 0;

    while (completed < n) {
        int selected = -1;

        for (int i = 0; i < n; i++) {
            if (proc[i].remaining_time <= 0) continue;
            if (proc[i].arrival_time > current_time) continue;

            if (selected == -1) {
                selected = i;
            } else {
                if (proc[i].remaining_time < proc[selected].remaining_time) {
                    selected = i;
                } else if (proc[i].remaining_time == proc[selected].remaining_time &&
                           proc[i].arrival_time < proc[selected].arrival_time) {
                    selected = i;
                }
            }
        }

        if (selected == -1) { current_time++; continue; }

        if (proc[selected].first_run == 0) {
            proc[selected].response_time = current_time;
            proc[selected].first_run     = 1;
        }

        if (proc[selected].pid != prev_pid) {
            gantt[gantt_size]      = proc[selected].pid;
            gantt_time[gantt_size] = current_time;
            gantt_size++;
            prev_pid = proc[selected].pid;
        }

        proc[selected].remaining_time--;
        current_time++;

        if (proc[selected].remaining_time == 0) {
            proc[selected].completion_time = current_time;
            completed++;
        }
    }

    gantt_time[gantt_size] = current_time;

    calculate_metrics(proc, n);

    printf(",\"srtf\": {");
    printf("\"gantt\": [");
    for (int i = 0; i < gantt_size; i++) {
        printf("{\"pid\": %d, \"start\": %d, \"end\": %d}",
               gantt[i], gantt_time[i], gantt_time[i+1]);
        if (i < gantt_size - 1) printf(",");
    }
    printf("],");

    printf("\"processes\": [");
    for (int i = 0; i < n; i++) {
        printf("{\"pid\": %d, \"at\": %d, \"bt\": %d,"
               "\"wt\": %d, \"tat\": %d, \"rt\": %d}",
               proc[i].pid, proc[i].arrival_time, proc[i].burst_time,
               proc[i].waiting_time, proc[i].turnaround_time,
               proc[i].response_time);
        if (i < n - 1) printf(",");
    }

    float twt = 0, ttat = 0, trt = 0;
    for (int i = 0; i < n; i++) {
        twt  += proc[i].waiting_time;
        ttat += proc[i].turnaround_time;
        trt  += proc[i].response_time;
    }
    printf("],\"avg_wt\": %.2f, \"avg_tat\": %.2f, \"avg_rt\": %.2f}}",
           twt/n, ttat/n, trt/n);
}

void print_gantt_sjf(int *gantt, int *gantt_time, int size) {
    printf("\nGantt Chart (SJF):\n");
    printf("+");
    for (int i = 0; i < size; i++) printf("------+");
    printf("\n|");
    for (int i = 0; i < size; i++) printf(" P%-3d |", gantt[i]);
    printf("\n+");
    for (int i = 0; i < size; i++) printf("------+");
    printf("\n");
    for (int i = 0; i <= size; i++) printf("%-7d", gantt_time[i]);
    printf("\n");
}