#define _GNU_SOURCE
#include "process.h"
#include <sched.h>
//#include <errno.h>
#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <time.h>
#define GET_TIME 334
#define PRINTK 333

int proc_assign_cpu(int pid, int core)
{
	if (core > sizeof(cpu_set_t)) {
		fprintf(stderr, "Assign Cpu Error.");
		return -1;
	}

	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);

	if (sched_setaffinity(pid, sizeof(mask), &mask) < 0) {
		perror("sched_setaffinity error");
		exit(1);
	}

	return 0;
}

int proc_exec(struct process proc) // struct: process is defined in process.h
{
	int pid = fork();

	if (pid < 0) {
		perror("Error pid < 0");
		return -1;
	}
	
	if(pid == 0){
			// w/ syscall
			long s_time, s_ntime, e_time, e_ntime;
			syscall(GET_TIME, &s_time, &s_ntime);
			// 執行 proc.t_exec 個unit time
			for(int i = 0; i < proc.t_exec; ++i){
				UNIT_T();
			}
			syscall(GET_TIME, &e_time, &e_ntime);
			// printf("[project1] %d %ld.%09ld %ld.%09ld\n", getpid(), s_time, s_ntime, e_time, e_ntime);
			char msg_to_printk[200];
			sprintf(msg_to_printk, "[project1] %d %ld.%09ld %ld.%09ld\n", getpid(), s_time, s_ntime, e_time, e_ntime);
			syscall(PRINTK, msg_to_printk);
			exit(0);
			
			// //For Test
			
			// clock_t s_time, e_time;
			// s_time = clock();
			// for(int i = 0; i < proc.t_exec; ++i){
			// 	UNIT_T();
			// }
			// e_time = clock();
			// printf("[project1] %d %lu %lu\n", getpid(), s_time, e_time);
			// exit(0);
	}

	/* Assign child to another core prevent from interupted by parant *///
	//proc_assign_cpu(pid, CHILD_CPU);
	
	return pid;
}

int proc_block(int pid)
{
	struct sched_param param;
	
	/* SCHED_IDLE should set priority to 0 */
	param.sched_priority = 0;

	int ret = sched_setscheduler(pid, SCHED_IDLE, &param);
	//int ret = sched_setscheduler(pid, 0, &param);
	// sched_setscheduler()函数将pid所指定进程的调度策略和调度参数分别设置为param指向的sched_param结构中指定的policy和参数。
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	return ret;
}

int proc_wakeup(int pid)
{
	struct sched_param param;
	
	/* SCHED_OTHER should set priority to 0 */
	param.sched_priority = 0;

	int ret = sched_setscheduler(pid, SCHED_OTHER, &param);
	//int ret = sched_setscheduler(pid, 0, &param);
	
	if (ret < 0) {
		perror("sched_setscheduler");
		return -1;
	}

	return ret;
	return 0;
}
