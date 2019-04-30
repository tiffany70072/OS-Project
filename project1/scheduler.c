#include <stdio.h>
#include <stdlib.h>
#include <sched.h>
#include "process.h"
#include "scheduler.h"

static int timeLastCS;    // Last context switch time for RR scheduling
static int timeNow;       // Current unit time
static int curRunning;    // Index of running process. -1 if no process running
static int lastRunning;   // Index of last running process. -1 if no process running
static int procFinishCnt; // Number of finish Process

// Sort processes by ready time
int cmp(const void *a, const void *b) {
	return ((struct process *)a)->timeReady - ((struct process *)b)->timeReady;
}

// Return index of next process
int NextProcess(struct process *proc, int procNum, int policy){
	////////////////////
	// Initialization //
	////////////////////
	int ret = -1;

	///////////////////////////
	// Non-preemptive policy //
	///////////////////////////
	if (curRunning != -1 && (policy == POLICY_SJF || policy == POLICY_FIFO)){
		// 對於 SJF and FIFO，只要有人正在跑，就讓他繼續跑，直接回傳正在跑的人的編號
		return curRunning; 
	}

	//////////////////////////////////////////////////////
	// According to "policy", choose different strategy //
	//////////////////////////////////////////////////////
	if (policy == POLICY_PSJF || policy ==  POLICY_SJF) {
		for (int i = 0 ; i < procNum ; i++) {
			if (proc[i].pid == -1 || proc[i].timeExec == 0){
				// 如果這個 process 已經跑完了就不考慮他
				continue;
			}
			if (ret == -1 || proc[i].timeExec < proc[ret].timeExec){
				// 從沒跑完的中，去挑一個目前為止剩下的執行時間最短的，而且只挑一次（挑過的話 ret != -1)
				ret = i;
			}
		}
	} else if (policy == POLICY_FIFO) {
		for(int i = 0 ; i < procNum ; i++) {
			if(proc[i].pid == -1 || proc[i].timeExec == 0){
				// 如果這個 process 已經跑完了就不考慮他
				continue;
			}
			if(ret == -1 || proc[i].timeReady < proc[ret].timeReady){
				// 從全部沒跑完的檢查一遍，找出 t_ready 最小的（最早到的）
				ret = i;
			}
		}
	} else if (policy == POLICY_RR) {
		if (curRunning == -1 && lastRunning == -1) {
			// 如果沒人在跑
			for (int i = 0 ; i < procNum ; i++) {
				if (proc[i].pid != -1 && proc[i].timeExec > 0){
					// 找還沒跑完 (t_exec > 0)，且準備好要跑的人的最小的，這應該是初始
					ret = i;
					break;
				}
			}
		} else if ((timeNow - timeLastCS) % 500 == 0 || (curRunning == -1 && lastRunning != -1)) {
			// 每個 process 分到 500 就要換人
			ret = (lastRunning + 1) % procNum;
			// 換的方式就是找編號是下一個的
			int cnt = 0;
			while (proc[ret].pid == -1 || proc[ret].timeExec == 0){ // 如果編號下一個人跑完了，就再往下找一個
				ret = (ret + 1) % procNum;
				if (cnt == procNum) {
					ret = -1;
					break;
				}
				cnt = cnt + 1;
			}
		} else {
			ret = curRunning;
		}
	}
	return ret;
}

int Scheduling(struct process *proc, int procNum, int policy){
	qsort(proc, procNum, sizeof(struct process), cmp); // 利用 timeReady 來排序
	// void qsort(void* base, size_t n, size_t size, int (*cmp)(const void*, const void*))：陣列基礎的快速排序法函數
	// 陣列是參數 base，n 是陣列大小，size 是每個元素的大小，最後的參數是指向函數的指標，這是比較元素大小的函數( 即上面 compare() 函數）。

	//////////////////////////////////////
	// Initial pid = -1 imply not ready //
	// + set all pids as -1             //
	//////////////////////////////////////
	for (int i = 0 ; i < procNum ; i++){
		proc[i].pid = -1;
	}

	////////////////////////////////////
	// Set high priority to scheduler //
	////////////////////////////////////
	ProcWakeup(getpid(), 10);
	
	///////////////////////
	// Initial scheduler //
	///////////////////////
	timeNow       = 0;
	curRunning    = -1; // 一開始在跑的 pid 是 -1，表示沒人在跑
	lastRunning   = -1;
	procFinishCnt = 0;  // 一開始完成的 process 數目是零
	
	while(1) {
		//fprintf(stderr, "Current time: %d\n", timeNow);

		////////////////////////////////////////
		// Check if curRunning process finish //
		////////////////////////////////////////

		// 如果有人在跑，而且這個人剛好跑完了
		if (curRunning != -1 && proc[curRunning].timeExec == 0) {
			// waitpid()会暂时停止目前进程的执行, 直到有信号来到或子进程结束. 
			waitpid(proc[curRunning].pid, NULL, 0);
			printf("%s %d\n", proc[curRunning].name, proc[curRunning].pid);
			lastRunning = curRunning;
			curRunning = -1;
			procFinishCnt++; // 有一個 process 跑完了
		}

		//////////////////////////////////////////
		// Check whether all processes finished //
		//////////////////////////////////////////
		if (procFinishCnt == procNum){
			break;
		}

		//////////////////////////////////
		// If process ready, execute it //
		//////////////////////////////////
		for (int i = 0 ; i < procNum ; i++) {
			if (proc[i].timeReady == timeNow) {
				proc[i].pid = ProcExec(proc[i], getpid());
			}
		}

		/////////////////////////////////
		// Select next running process //
		/////////////////////////////////

		// 得到下一個時間點能跑的 process 的編號
		int procNext = NextProcess(proc, procNum, policy);
		//printf("%d\n", procNext);
		if (procNext != -1) {
			ProcWakeup(proc[procNext].pid, 2);
			ProcBlock(getpid());
			// Context switch
			if (curRunning != procNext) { // 如果上下兩個時間點對應到的 process 不一樣的話
				lastRunning = procNext;
				curRunning = procNext;
				timeLastCS = timeNow;
			}
		}

		/////////////////////////
		// Run an unit of time //
		/////////////////////////
		UNIT_T();
		if (curRunning != -1) {
			// 如果有人在跑，那這個人可以分配到執行一個單位的時間
			proc[curRunning].timeExec--;
		}
		timeNow++;
	}
	return 0;
}