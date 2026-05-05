#ifndef SJF_H
#define SJF_H
 
#include "process.h"
 
void sjf_scheduling(Process *processes, int n);
void srtf_scheduling(Process *processes, int n);
void print_gantt_sjf(int *gantt, int *gantt_time, int size);
 
#endif
 