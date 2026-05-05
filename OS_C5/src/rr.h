#ifndef RR_H
#define RR_H
 
#include "process.h"
 
void rr_scheduling(Process *processes, int n, int quantum);
void print_gantt_rr(int *gantt, int *gantt_time, int size);
 
#endif
 