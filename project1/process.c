#define _GNU_SOURCE
#include "process.h"
#include <sched.h>
//#include <errno.h>
#include <stdio.h>
//#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include<sys/resource.h>
#include <time.h>
#include<signal.h>
//#define GET_TIME 314
#define PRINTK 333
#define BILLION 1E9
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

int proc_exec(struct process proc,int policy) // struct: process is defined in process.h
{
    int pid_parent = getpid();
    int pid = fork();

    if (pid < 0) {
        perror("Error pid < 0");
        return -1;
    }
    
    if(pid == 0){
            
            
            //For Test
            char to_dmesg[200];
            struct timespec s_time, e_time;
            unsigned long s_t,e_t;
            //printf("%d is stop by SIGSTOP\n",getpid()); 
            printf("%d is start....\n",getpid());
            kill(getpid(),SIGSTOP);
            clock_gettime(CLOCK_REALTIME,&s_time);
            for(int i = 0; i < proc.t_exec; ++i){
                kill(pid_parent,SIGCONT);
                //printf("%d is stop by SIGSTOP\n",getpid());
                if (i!= proc.t_exec - 1)
                    kill(getpid(),SIGSTOP); 
            }
            clock_gettime(CLOCK_REALTIME,&e_time);
            sprintf(to_dmesg, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), s_time.tv_sec, s_time.tv_nsec, e_time.tv_sec, e_time.tv_nsec);
            //sprintf(to_dmesg,"[project1] %d %lu %lu\n", getpid(), s_t, e_t);
            printf("[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), s_time.tv_sec, s_time.tv_nsec, e_time.tv_sec, e_time.tv_nsec);
            syscall(PRINTK,to_dmesg);   
            printf("%d is ending....\n",getpid());
            exit(0);
    }
    /* Assign child to another core prevent from interupted by parant *///
    else{
            proc_assign_cpu(pid,1);  
            /* 
            struct sched_param param;
            int maxpri;
            if (policy==1){
                maxpri = sched_get_priority_max(SCHED_FIFO);
                param.sched_priority = maxpri;
                if (sched_setscheduler(pid,SCHED_FIFO,&param)<0)
                {
                    perror("sched_setscheduler() failed");
                    exit(1);
                }
            }
            else if (policy==2){
                maxpri = 99;
                param.sched_priority = maxpri;
                if (sched_setscheduler(pid,SCHED_RR,&param)<0)
                {
                    perror("sched_setscheduler() failed");
                    exit(1);
                }
            }
            else if (policy==3){
                maxpri = proc.t_exec;
                if (setpriority(PRIO_PROCESS,pid,maxpri)<0)
                {
                    
                    perror("schedpriority() failed");
                    exit(1);
                }
            }
            */
            waitpid(pid,0,WUNTRACED);
            
    }
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
