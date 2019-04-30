#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>

// Running one unit time
#define UNIT_T() {volatile unsigned long i; for (i = 0; i < 1000000UL; i++);}

struct process {
	char name[32];
	int timeReady;
	int timeExec;
	pid_t pid;
};

// Execute the process and return pid
int ProcExec(struct process proc, pid_t parentPID);

// Set very low priority tp process
int ProcBlock(int pid);

// Set high priority to process
int ProcWakeup(int pid, int priority);

#endif