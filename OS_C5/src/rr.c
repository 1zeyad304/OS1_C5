#include <stdio.h>
#include <string.h>
#include "rr.h"
#include "metrics.h"
 
void rr_scheduling(Process *processes, int n, int quantum) {
    Process proc[n];
    memcpy(proc, processes, n * sizeof(Process));
 
    for (int i = 0; i < n; i++) {
        proc[i].remaining_time = proc[i].burst_time;
        proc[i].first_run      = 0;
        proc[i].completion_time = 0;
        proc[i].response_time  = -1;
    }
 
    int gantt[10000];
    int gantt_time[10001];
    int gantt_size = 0;
 
    /* Ready queue */
    int queue[10000];
    int in_queue[n];
    int q_front = 0, q_rear = 0;
    memset(in_queue, 0, sizeof(in_queue));
 
    int current_time = 0;
    int completed    = 0;
 
    /* Enqueue processes that arrive at time 0 */
    for (int i = 0; i < n; i++) {
        if (proc[i].arrival_time == 0) {
            queue[q_rear++] = i;
            in_queue[i] = 1;
        }
    }
 
    gantt_time[0] = 0;
 
    while (completed < n) {
 
        /* If queue is empty, advance time to next arrival */
        if (q_front == q_rear) {
            int next_arrival = -1;
            for (int i = 0; i < n; i++) {
                if (proc[i].remaining_time > 0 && !in_queue[i]) {
                    if (next_arrival == -1 || proc[i].arrival_time < next_arrival)
                        next_arrival = proc[i].arrival_time;
                }
            }
            if (next_arrival == -1) break;
            current_time = next_arrival;
            for (int i = 0; i < n; i++) {
                if (proc[i].remaining_time > 0 && !in_queue[i] &&
                    proc[i].arrival_time <= current_time) {
                    queue[q_rear++] = i;
                    in_queue[i] = 1;
                }
            }
            continue;
        }
 
        int idx = queue[q_front++];
 
        /* Record first response */
        if (proc[idx].response_time == -1) {
            proc[idx].response_time = current_time;
        }
 
        /* How long does this process run? */
        int run_time = (proc[idx].remaining_time < quantum)
                       ? proc[idx].remaining_time
                       : quantum;
 
        /* Gantt entry */
        gantt[gantt_size]          = proc[idx].pid;
        gantt_time[gantt_size]     = current_time;
        gantt_time[gantt_size + 1] = current_time + run_time;
        gantt_size++;
 
        proc[idx].remaining_time -= run_time;
        current_time             += run_time;
 
        /* Enqueue any newly arrived processes */
        for (int i = 0; i < n; i++) {
            if (!in_queue[i] && proc[i].remaining_time > 0 &&
                proc[i].arrival_time <= current_time) {
                queue[q_rear++] = i;
                in_queue[i] = 1;
            }
        }
 
        if (proc[idx].remaining_time == 0) {
            proc[idx].completion_time = current_time;
            completed++;
        } else {
            /* Re-enqueue the process */
            queue[q_rear++] = idx;
        }
    }
 
    gantt_time[gantt_size] = current_time;
 
    calculate_metrics(proc, n);
 
    /* Build ready queue snapshot string for display */
    /* We emit JSON output identical in structure to C1 */
    printf("{\"rr\": {");
    printf("\"quantum\": %d,", quantum);
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
 
void print_gantt_rr(int *gantt, int *gantt_time, int size) {
    printf("\nGantt Chart (Round Robin):\n");
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
