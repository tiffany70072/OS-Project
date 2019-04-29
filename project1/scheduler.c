//#define _GNU_SOURCE
#include "process.h"
#include "scheduler.h"
#include <stdlib.h>
//#include <signal.h>
#include <stdio.h>
//#include <unistd.h>
//#include <sys/wait.h> // wait, waitpid
#include <sched.h>

/* Last context switch time for RR scheduling */
static int t_last;

/* Current unit time */
static int ntime;

/* Index of running process. -1 if no process running */
static int running;
static int last_running;

/* Number of finish Process */
static int finish_cnt;

/* Sort processes by ready time */
int cmp(const void *a, const void *b) {
	return ((struct process *)a)->t_ready - ((struct process *)b)->t_ready;
}

/* Return index of next process  */
int next_process(struct process *proc, int nproc, int policy)
{
	/* Non-preemptive */
	if (running != -1 && (policy == SJF || policy == FIFO))
		return running; // 對於 SJF and FIFO，只要有人正在跑，就讓他繼續跑，直接回傳正在跑的人的編號

	int ret = -1;

	if (policy == PSJF || policy ==  SJF) {
		for (int i = 0; i < nproc; i++) {
			//printf("%d ", proc[i].t_exec);
			if (proc[i].pid == -1 || proc[i].t_exec == 0) 			// 如果這個 process 已經跑完了就不考慮他
				continue;
			if (ret == -1 || proc[i].t_exec < proc[ret].t_exec) 	// 從沒跑完的中，去挑一個目前為止剩下的執行時間最短的，而且只挑一次（挑過的話 ret != -1)
				ret = i;
		}
		//printf("\n");
	}

	else if (policy == FIFO) { 										// 在 FIFO，剛好遇到前一個人跑完
		for(int i = 0; i < nproc; i++) {
			if(proc[i].pid == -1 || proc[i].t_exec == 0)
				continue;
			if(ret == -1 || proc[i].t_ready < proc[ret].t_ready) 	// 從全部沒跑完的檢查一遍，找出 t_ready 最小的（最早到的）
				ret = i;
		}
    }

	else if (policy == RR){
		if (running == -1 && last_running == -1) { 										// 如果沒人在跑
			for (int i = 0; i < nproc; i++) {
				if (proc[i].pid != -1 && proc[i].t_exec > 0){       // 找還沒跑完 (t_exec > 0)，且準備好要跑的人的最小的，這應該是初始
					ret = i;
					break;
				}
			}
		}
		else if ((ntime - t_last) % 500 == 0 || (running == -1 && last_running != -1))  { 					// 每個 process 分到 500 就要換人
			ret = (last_running + 1) % nproc; 
			int cnt = 0;							// 換的方式就是找編號是下一個的
			while (proc[ret].pid == -1 || proc[ret].t_exec == 0){ 	// 如果編號下一個人跑完了，就再往下找一個
				ret = (ret + 1) % nproc;
				cnt++;
				if(cnt==nproc){
					ret = -1;
					break;				
				}
			}
		}
		else
			ret = running; 		
	}
	return ret;
}

int scheduling(struct process *proc, int nproc, int policy)
{
	qsort(proc, nproc, sizeof(struct process), cmp); // 利用 t_ready 來排序
	// void qsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*))：陣列基礎的快速排序法函數
	// 陣列是參數 base，n 是陣列大小，size 是每個元素的大小，最後的參數是指向函數的指標，這是比較元素大小的函數( 即上面 compare() 函數）。

	/* Initial pid = -1 imply not ready */ // 初始化所有 process 的 pid = -1
	for (int i = 0; i < nproc; i++)
		proc[i].pid = -1;

	/* Set single core prevent from preemption */
	// getpid(): 用来取得目前进程的进程识别码，许多程序利用取到的此值来建立临时文件， 以避免临时文件相同带来的问题。
	//proc_assign_cpu(getpid(), PARENT_CPU); 


	/* Set high priority to scheduler */
	proc_wakeup(getpid(), 2);
	
	/* Initial scheduler */
	ntime = 0;
	running = -1; 	// 一開始在跑的 pid 是 -1，表示沒人在跑
	last_running = -1;
	finish_cnt = 0; // 一開始完成的 process 數目是零
	
	while(1) {
		//fprintf(stderr, "Current time: %d\n", ntime);

		/* Check if running process finish */
		if (running != -1 && proc[running].t_exec == 0) { 		// 如果有人在跑，而且這個人剛好跑完了
		

			//fprintf(stderr, "%s finish at time %d.\n", proc[running].name, ntime);

			//kill(running, SIGKILL);
			waitpid(proc[running].pid, NULL, 0); 				// waitpid()会暂时停止目前进程的执行, 直到有信号来到或子进程结束. 
			printf("%s %d\n", proc[running].name, proc[running].pid);
			last_running = running;
			running = -1;
			finish_cnt++; 										// 有一個 process 跑完了

			/* All process finish */
			if (finish_cnt == nproc)
				break;
		}

		/* Check if process ready and execute */
		for (int i = 0; i < nproc; i++) {
			if (proc[i].t_ready == ntime) {
				proc[i].pid = proc_exec(proc[i], getpid()); 		
				//proc_block(proc[i].pid);
//#ifdef DEBUG
				//fprintf(stderr, "%s ready at time %d.\n", proc[i].name, ntime);
//#endif
			}

		}

		/* Select next running process */
		int next = next_process(proc, nproc, policy); 	// 得到下一個時間點能跑的 process 的編號
		//printf("%d\n", next);
		if (next != -1) {
			proc_wakeup(proc[next].pid, 2);
			proc_block(getpid());
			/* Context switch */
			if (running != next) { 						// 如果上下兩個時間點對應到的 process 不一樣的話
				//printf("context %d, %d\n", running, next);				
				//proc_block(proc[running].pid);
				//proc_wakeup(proc[next].pid, 2);
				//proc_block(proc[running].pid);
				last_running = next;
				running = next;
				t_last = ntime;
			}
		}

		/* Run an unit of time */
		UNIT_T();
		if (running != -1) 								// 如果有人在跑，那這個人可以分配到執行一個單位的時間
			proc[running].t_exec--;
		ntime++;
	}

	return 0;
}

