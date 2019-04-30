#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include "process.h"
#define PRINTK 333
#define GET_TIME 334

int ProcExec(struct process proc, pid_t parentPID){
	//////////////////////////
	// Fork a child process //
	//////////////////////////
	int pid = fork();
	if (pid < 0) {
		perror("Error pid < 0");
		return -1;
	}

	///////////////////////////////////////////////////
	// Do something if this process is child process //
	///////////////////////////////////////////////////
	if(pid == 0){
			// w/ syscall
			long s_time, s_ntime, e_time, e_ntime;
			//printf("block : %d\n",getpid());
			ProcBlock(getpid());
			//printf("awake : %d\n",getpid());
			syscall(GET_TIME, &s_time, &s_ntime);
			// 執行 proc.t_exec 個unit time
			for (int i = 0 ; i < proc.timeExec ; i++){
				ProcWakeup(parentPID, 10);
				if (i != proc.timeExec-1) {
					ProcBlock(getpid());	
				}
			}
			syscall(GET_TIME, &e_time, &e_ntime);
			// printf("[project1] %d %ld.%09ld %ld.%09ld\n", getpid(), s_time, s_ntime, e_time, e_ntime);
			char msgToPrintk[200];
			sprintf(msgToPrintk, "[project1] %d %ld.%09ld %ld.%09ld\n", getpid(), s_time, s_ntime, e_time, e_ntime);
			syscall(PRINTK, msgToPrintk);
			
			exit(0);
	}
	return pid;
}

int ProcBlock(int pid){
	struct sched_param param;	

	param.sched_priority = 1;
	//printf("block : %d %d \n", pid, param.sched_priority);

	// sched_setscheduler() 函数将pid所指定进程的调度策略和调度参数分别设置为param指向的sched_param结构中指定的policy和参数。
	int ret = sched_setscheduler(pid, SCHED_FIFO, &param);
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}
	return ret;
}

int ProcWakeup(int pid, int priority){
	struct sched_param param;

	param.sched_priority = priority;
	//printf("wakeup : %d %d \n", pid, param.sched_priority);

	// sched_setscheduler() 函数将pid所指定进程的调度策略和调度参数分别设置为param指向的sched_param结构中指定的policy和参数。
	int ret = sched_setscheduler(pid, SCHED_FIFO, &param);
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}
	return ret;
}