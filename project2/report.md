# Operating System Project 2 Report
## 1. Programming design

### 題目要求
* 本次目標是在兩個 users 之間傳輸資料，並比較 file I/O (read and write) 和 mmap 兩種傳輸方式的差異
* 在 master 端的 user1 將資料傳送給在 slave 端的 user2
* 資料傳送的過程需經過：master program -> master device -> user device -> user program
### mmap 作法
* 利用虛擬地址直接存取檔案
* Kernel 版本 = 4.15.0
#### Master
* master 端是傳送端，因此他已事先知道檔案的大小，能直接把整個檔案映射到虛擬位址上，讓 master device 直接透過映射位址讀取檔案內容。這個方法可以省去原本 file I/O 需要的 buffer，因此可以少一次搬移資料的時間，但是建立 mmap 本身有 overhead
* 需要配合 3 個 kernel functions

|socket id | socket name| function|
|----------|------------|---------|
|0x12345677|master_IOCTL_CREATESOCK|create socket and accept a connection|
|0x12345678|master_IOCTL_MMAP|execute MMAP: write data
|0x12345679|master_IOCTL_EXIT|exit the socket

#### Slave
* 由於slave端事先不知道檔案大小，因此映射時需要先開啟一給定的大小（通常是 page size 的整數倍)，如果滿了就在開新的，直到檔案傳完為止
* 但檔案本身原本是空的，如果直接把開啟的檔案拿來映射，會導致 bus error。因此每次要開新的映射位址時，需要先在該檔案的對應 offset 位置寫入一個 null character。並於檔案傳送完畢後將多出來的 offset truncate掉
* 需要設定 IP 位址
* 需要配合 2 個 kernel functions

|socket id | socket name| function|
|----------|------------|---------|
|0x12345677|slave_IOCTL_CREATESOCK|create socket and accept a connection|
|0x12345678|slave_IOCTL_MMAP|execute MMAP: receive data and return received data
|0x12345679|slave_IOCTL_EXIT|exit the socket

#### Structure
![](https://github.com/tiffany70072/OS-Project/blob/master/project2/graph/structure.png?raw=true)


### 程式架構
 
```
./ksocket: 
    the device module including the functions used for kernel socket
    BSD-style socket APIs
./master_device : the device module for master server
./slave_device  : the device module for slave client
./user_program : including user program "master" and "slave"
```

### 執行方式
Compile
```
$ sudo su
$ bash compile.sh
$ bash createDummyFiles.sh
```
Run
```
$ cd user_program
$ bash testrun.sh f > ../results/testrun_f_1.txt
$ bash testrun.sh f > ../results/testrun_f_2.txt
$ bash testrun.sh f > ../results/testrun_f_3.txt
$ bash testrun.sh f > ../results/testrun_f_4.txt
$ bash testrun.sh f > ../results/testrun_f_5.txt
$ bash testrun.sh m > ../results/testrun_m_1.txt
$ bash testrun.sh m > ../results/testrun_m_2.txt
$ bash testrun.sh m > ../results/testrun_m_3.txt
$ bash testrun.sh m > ../results/testrun_m_4.txt
$ bash testrun.sh m > ../results/testrun_m_5.txt
```




## 2. The Result
* 使用助教所提供的sample_code.zip中的data資料夾中的測試資料，每筆測試資料皆用fcntl for file I/O及mmap for memory-mapped I/O測試五遍
### 2.1. file1_in.txt(4 bytes)

| No | fcntl | mmap |
| -------- | -------- | -------- | 
| 1     | 0.05995     | 0.0755     | 
| 2     | 0.0231     | 0.02925     | 
| 3     | 0.03945     | 0.05365     | 
| 4     | 0.0476     | 0.02325     |
| 5     | 0.02225     | 0.0263     |
| Average     | 0.03847     | 0.04159     |

### 2.2. file2_in.txt(577 bytes)

| No | fcntl | mmap |
| -------- | -------- | -------- | 
| 1     | 0.69205     | 0.0327     | 
| 2     | 0.02355     | 0.02285     | 
| 3     | 0.02445     | 0.0412     | 
| 4     | 0.0937     | 0.4199     |
| 5     | 0.02315     | 0.0267     |
| Average     | 0.17138     | 0.10867     |

### 2.3. file3_in.txt(9695 bytes)

| No | fcntl | mmap |
| -------- | -------- | -------- | 
| 1     | 0.40045     | 0.0453     | 
| 2     | 0.1141     | 0.05325     | 
| 3     | 0.09975     | 0.05965     | 
| 4     | 0.0603     | 0.05175     |
| 5     | 0.0767     | 0.0853     |
| Average     | 0.15026     | 0.05905     |
### 2.4. file4_in.txt(1502860 bytes)

| No | fcntl | mmap |
| -------- | -------- | -------- | 
| 1     | 5.4051     | 3.41125     | 
| 2     | 4.20445     | 3.3897     | 
| 3     | 4.14045     | 3.79245     | 
| 4     | 4.39445     | 3.607     |
| 5     | 4.3738     | 2.61685     |
| Average     | 4.50365     | 3.36345     |


## 3. The comparison the performance between file I/O and memory-mapped I/O, and explain why.
* 除了助教所提供的測試資料外，我們還另外建立了資料大小由125bytes到100,000,000bytes的24份測試資料，每份測試資料分別跑了file I/O及memory-mapped I/O各3遍並平均Transmission time，得出的結果如下圖：

![](https://github.com/tiffany70072/OS-Project/blob/master/project2/graph/valid_graph.png?raw=true)


* 造成差異的原因
    * 由Result及上圖可看出，在File size較大時，mmap的Transmission time較短，fcntl的Transmission time較長，而File size較小時，差異比較不明顯。推測原因是因為系統在處理fcntl時，會先檢查page cache裡面有沒有資料，若沒有會copy一份資料到buffer，而使用mmap時，會先造成page fault，然後系統會將VMM對應的page存取到記憶體中。當處理大檔案及重複存取該檔案時，由於可以少copy一次資料到buffer，並且可以避免page fault，因此mmap的Transmission time會較短。
    * 在實驗結果中，處理小檔案時mmap的Transmission time較為浮動，這或許是由於mmap所造成的page fault極耗資源，因此mmap的初始化代價較大，相比之下處理小檔案的時候fcntl速度較穩定。

## 4. Work list of team members
| 學號 | 姓名 | 負責工作 |
| -------- | -------- | -------- |
| r07922059 | 陳毅 | Program |
| r07922035 | 李漪莛 | 產生圖表、撰寫報告 |
| r07922135 | 顏百謙 | 製造數據、分析結果 |
| r07922119 | 吳達懿 | Program |
| r06922002 | 姚嘉昇 | 製造數據、分析結果 |
| r07922117 | 黃榆珊 | 產生圖表、撰寫報告 |
