#ifndef _SCHEDULING_H_
#define _SCHEDULING_H_

#include "process.h"

#define POLICY_FIFO 1
#define POLICY_RR   2
#define POLICY_SJF  3
#define POLICY_PSJF 4

// Return index of next process
int NextProcess(struct process *proc, int procNum, int policy);

// Running scheduler
int Scheduling(struct process *proc, int procNum, int policy);

#endif
