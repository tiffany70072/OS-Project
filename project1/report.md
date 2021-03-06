# Operating System Project 1 Report
## 1. 設計
### 1.1. main.c
* 根據input資料決定schedule policy
* 根據input資料決定有多少process要跑
* 依照schedule policy呼叫schedule.c不同的排程方法

### 1.2. process.c
* `#define UNIT_T()`: Define running one unit time
* `int proc_assign_cpu(int pid, int core)`: Assign process to specific core
* `int proc_exec(struct process proc, pid_t parent_pid)`: Execute the process and return pid
* `int proc_block(int pid)`: Set very low priority tp process
* `int proc_wakeup(int pid, int priority)`: Set high priority to process

### 1.3. schedule.c
* `int next_process(struct process *proc, int nproc, int policy)`: Return next process to run
    1. Non-preemptive: 對於 SJF and FIFO，只要有人正在跑，就讓他繼續跑，直接回傳正在跑的人的編號
    2. PSJF || SJF: 如果這個 process 已經跑完了就不考慮他，從沒跑完的中，去挑一個目前為止剩下的執行時間最短的，而且只挑一次（挑過的話 ret != -1）
    3. FIFO: 在FIFO，剛好遇到前一個人跑完，從全部沒跑完的檢查一遍，找出 t_ready 最小的（最早到的）
    4. RR
        * 如果沒人在跑，找還沒跑完 (t_exec > 0)，且準備好要跑的人的最小的，這應該是初始
        * 每個 process 分到 500 就要換人，換的方式就是找編號是下一個的，如果編號下一個人跑完了，就再往下找一個
* `int scheduling(struct process *proc, int nproc, int policy)`: Running scheduler
    1. 利用 t_ready 來排序
    2. 初始化所有 process 的 pid = -1
    3. Set high priority to scheduler
    4. Initial scheduler
    5. while(1) {
        1. Check if running process finish
        2. Check if process ready and execute
        3. Select next running process
        4. Run an unit of time
        }

## 2. 執行範例測資的結果
### 2.1. FIFO
#### 2.1.1 FIFO_1.txt
```
FIFO
5
P1 0 500
P2 0 500
P3 0 500
P4 0 500
P5 0 500
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/FIFO_1.png)
#### 2.1.2 FIFO_2.txt
```
FIFO
4
P1 0 80000
P2 100 5000
P3 200 1000
P4 300 1000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/FIFO_2.png)
#### 2.1.3 FIFO_3.txt
```
FIFO
7
P1 0 8000
P2 200 5000
P3 300 3000
P4 400 1000
P5 500 1000
P6 500 1000
P7 600 4000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/FIFO_3.png)
#### 2.1.4 FIFO_4.txt
```
FIFO
4
P1 0 2000
P2 500 500
P3 500 200
P4 1500 500
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/FIFO_4.png)
#### 2.1.5 FIFO_5.txt
```
FIFO
7
P1 0 8000
P2 200 5000
P3 200 3000
P4 400 1000
P5 400 1000
P6 600 1000
P7 600 4000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/FIFO_5.png)

### 2.2. RR
#### 2.2.1 RR_1.txt
```
RR
5
P1 0 500
P2 0 500
P3 0 500
P4 0 500
P5 0 500
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/RR_1.png)
#### 2.2.2 RR_2.txt
```
RR
2
P1 600 4000
P2 800 5000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/RR_2.png)
#### 2.2.3 RR_3.txt
```
RR
6
P1 1200 5000
P2 2400 4000
P3 3600 3000
P4 4800 7000
P5 5200 6000
P6 5800 5000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/RR_3.png)
#### 2.2.4 RR_4.txt
```
RR
7
P1 0 8000
P2 200 5000
P3 300 3000
P4 400 1000
P5 500 1000
P6 500 1000
P7 600 4000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/RR_4.png)
#### 2.2.5 RR_5.txt
```
RR
7
P1 0 8000
P2 200 5000
P3 200 3000
P4 400 1000
P5 400 1000
P6 600 1000
P7 600 4000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/RR_5.png)

### 2.3. SJF

#### 2.3.1 SJF_1.txt
```
SJF
4
P1 0 7000
P2 0 2000
P3 100 1000
P4 200 4000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/SJF_1.png)
#### 2.3.2 SJF_2.txt
```
SJF
5
P1 100 100
P2 100 4000
P3 200 200
P4 200 4000
P5 200 7000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/SJF_2.png)
#### 2.3.3 SJF_3.txt
```
SJF
8
P1 100 3000
P2 100 5000
P3 100 7000
P4 200 10
P5 200 10
P6 300 4000
P7 400 4000
P8 500 9000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/SJF_3.png)
#### 2.3.4 SJF_4.txt
```
SJF
5
P1 0 3000
P2 1000 1000
P3 2000 4000
P4 5000 2000
P5 7000 1000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/SJF_4.png)
#### 2.3.5 SJF_5.txt
```
SJF
4
P1 0 2000
P2 500 500
P3 1000 500
P4 1500 500
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/SJF_5.png)

### 2.4. PSJF

#### 2.4.1 PSJF_1.txt
```
PSJF
4
P1 0 10000
P2 1000 7000
P3 2000 5000
P4 3000 3000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/PSJF_1.png)
#### 2.4.2 PSJF_2.txt
```
PSJF
5
P1 0 3000
P2 1000 1000
P3 2000 4000
P4 5000 2000
P5 7000 1000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/PSJF_2.png)
#### 2.4.3 PSJF_3.txt
```
PSJF
4
P1 0 2000
P2 500 500
P3 1000 500
P4 1500 500
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/PSJF_3.png)
#### 2.4.4 PSJF_4.txt
```
PSJF
4
P1 0 7000
P2 0 2000
P3 100 1000
P4 200 4000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/PSJF_4.png)
#### 2.4.5 PSJF_5.txt
```
PSJF
5
P1 100 100
P2 100 4000
P3 200 200
P4 200 4000
P5 200 7000
```
![](https://github.com/tiffany70072/OS-Project/blob/master/project1/results/PSJF_5.png)

## 3. 比較實際結果與理論結果，並解釋造成差異的原因
* 以下表格中時間單位為Unit time(UT)，實際開始及結束時間是依80000UT花了152.93935秒，因此用1UT=0.001912秒來做換算

* FIFO (以FIFO_4.txt為例)

| Process | 理論開始時間 | 理論執行時間 |理論結束時間 | 實際開始時間 | 實際結束時間 |
| -------- | -------- | -------- | -------- | -------- | -------- |
| P1     | 0     | 2000     |  2000     |0.000      | 1997.589     |
| P2     | 2000     | 500     |  2500     |1997.694      | 2497.066     |
| P3     | 2500     | 200     |  2700     |2498.133      | 2705.779     |
| P4     | 2700     | 500     |  3200     |2706.841      | 3177.840     |

* RR (RR_2.txt為例)

| Process | 理論開始時間 | 理論執行時間 |理論結束時間 | 實際開始時間 | 實際結束時間 |
| -------- | -------- | -------- | -------- | -------- | -------- |
| P1     | 0     | 4000     | 7500     |0.000     | 7613.112     |
| P2     | 500     | 5000     | 9000     |510.612     | 9154.022     |

* SJF (SJF_5.txt為例)

| Process | 理論開始時間 | 理論執行時間 |理論結束時間 | 實際開始時間 | 實際結束時間 |
| -------- | -------- | -------- | -------- | -------- | -------- |
| P1     | 0     |  2000     | 2000     |0.000     | 2008.964     | 
| P2     | 2000     | 500     | 2500     |2009.064     | 2504.760     | 
| P3     | 2500     | 500     | 3000     |2506.370     | 3009.435     |
| P4     | 3000     | 500     | 3500     |3011.015     | 3536.313     | 

* PSJF (PSJF_1.txt為例)

| Process | 理論開始時間 | 理論執行時間 |理論結束時間 | 實際開始時間 | 實際結束時間 |
| -------- | -------- | -------- | -------- | -------- | -------- |
| P1     | 0     | 10000     | 25000     |0.000     | 25277.061     |
| P2     | 1000     |  7000    |16000     |1005.549     | 16011.757      | 
| P3     | 2000     | 5000     | 10000     |2004.864     | 9999.728     |
| P4     | 3000     | 3000     | 6000     |2975.816     | 5986.407     | 


* 造成差異的原因
    * 用for loop模擬出來的unit time與理論值並不會完全相同，因為每次執行迴圈時CPU所花費的時間不會完全一樣，所以實際執行時間與理論執行時間不會完全相同
    * 在切換Process的時候除了有context switch的時間外，還有scheduler在找尋next process時也須花費時間，因此process的實際開始時間與理論不同

## 4. 各組員的貢獻
| 學號 | 姓名 | 負責工作 |
| -------- | -------- | -------- |
| r07922059 | 陳毅 | User program |
| r07922035 | 李漪莛 | User program |
| r07922135 | 顏百謙 | User program |
| r07922119 | 吳達懿 | Syscall printk, synchronize process |
| r06922002 | 姚嘉昇 | Syscall getnstimeofday, report |
| r07922117 | 黃榆珊 | Syscall getnstimeofday, report |