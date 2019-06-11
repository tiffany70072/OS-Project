# Operating System Project 2 Report
## 1. Programming design



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