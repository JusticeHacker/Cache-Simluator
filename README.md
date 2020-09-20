# Cache-Simluator
Using C Language simulate cache with replacement type "FIFO" and "LRU"

## Compile 
在linux環境下用 ```gcc main.c -lm -o cache``` 將.c檔編譯成執行檔cache

## Input data
執行格式:
```
./cache cache_size block_size associativity "replace_policy" "file"
```
* cache_size: 8, 16, …, 256 (KB) 
* block_size :4, 8, 16, …, 128 (B) 
* associativity: 1 (direct-mapped), 2, 4, 8, f (fully associative) 
* replace-policy: FIFO, LRU
* file: gcc.din, spice.din 

### 關於測資 gcc.din & spice.din
* Label 0(Read Data): 用對應的Address從cache讀取資料。
* Label 1(Write Data): 寫入資料到對應的Address。
* Label 2(Read Instruction): 用對應的Address從cache讀取指令。
* Address 都是32bit長，圖中未滿32bit視為前面補16進位的0。 // Ex: 408ed4 實際上是00408ed4
#### Example:
![error](https://github.com/JusticeLeee/Cache-Simluator/blob/master/images/Example.png)

## Output data
* Input file (輸入檔名)
* Demand fetch (需要處理的指令數) // = Cache hit + Cache miss
* Cache hit (有在快取找到資料的指令數)
* Cache miss (沒有在快取找到資料所以需要另外處裡的指令數)  
* Miss rate (失誤比例，Cache miss / Demand fetch) (精準到四位小數)
* Read data (要讀資料的指令數，也就是lable 0數量)
* Write data (要寫資料的指令數，也就是lable 1數量)
* Bytes from Memory (總共從記憶體傳輸了多少byte到Cache)
* Byte to memory (總共從Cache傳輸了多少byte資料到記憶體)

## FlowChar
![error](https://github.com/JusticeLeee/Cache-Simluator/blob/master/images/Flowchar.png)

## Result
![error](https://github.com/JusticeLeee/Cache-Simluator/blob/master/images/result1.png?raw=true "result1")
![error](https://github.com/JusticeLeee/Cache-Simluator/blob/master/images/result2.png?raw=true "result2")



