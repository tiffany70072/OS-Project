#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "process.h"
#include "scheduler.h"

int main(int argc, char* argv[]){
	char policy[256];  // policy, ex: FIFO
	int procNum;       // process number, ex: 3
	scanf("%s%d", policy, &procNum); 	    
	//printf("%s %d\n", policy, procNum);
	
	struct process *proc;
	proc = (struct process *)malloc(procNum * sizeof(struct process));

	for (int i = 0; i < procNum; i++) {
		scanf("%s%d%d", proc[i].name, &proc[i].timeReady, &proc[i].timeExec); // P1, 1, 10
	}

	if (strcmp(policy, "FIFO") == 0) {
		Scheduling(proc, procNum, POLICY_FIFO);
	} else if (strcmp(policy, "RR") == 0) {
		Scheduling(proc, procNum, POLICY_RR);
	} else if (strcmp(policy, "SJF") == 0) {
		Scheduling(proc, procNum, POLICY_SJF);
	} else if (strcmp(policy, "PSJF") == 0) {
		Scheduling(proc, procNum, POLICY_PSJF);
	} else {
		fprintf(stderr, "Invalid policy: %s", policy);
	}

	return 0;
}