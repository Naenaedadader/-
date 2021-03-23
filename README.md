# hw
總共有 2個struct 分別為information 和dat

struct information 為紀錄 時間，經度和緯度 

struct dat 為記錄單一tag的資料，此tag收集的所有時間經度緯度存為 info ，
另外第二題所需要的時間範圍存為 rng。

第1.1題的主要function為ReadData

其目的為:

讀取原始資料並整理,即可由 info 的 size 得到tag 總數。

第1.2題的主要function為 CompRng

其目的為:

計算單個tag的時間範圍




ReadData 的內部function:

1. 先藉由176行的Readline function依特定格式讀取資料

2. 183~193行如果下一筆資料不是相同tag則利用pushDAT function 對read_dat進行整理存到D裡面,
pushDat 藉由 SortInfo 對tag內的時間經度緯度進行排序然後刪除重複資料以達到整理資料。


CompRng 的內部function:

1. 211行的 stotm function 為把時間資料轉成tm的時間格式

