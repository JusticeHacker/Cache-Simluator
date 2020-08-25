# Cache-Simluator
Using C Language simulate cache with replacement type "FIFO" and "LRU"

## Compile 
在linux環境下用 ```gcc main.c -lm -o cache``` 將.c檔編譯成執行檔cache

## About input
* cache_size: 8, 16, …, 256 (KB) 
* block_size :4, 8, 16, …, 128 (B) 
* associativity: 1 (direct-mapped), 2, 4, 8, f (fully associative) 
* replace-policy: FIFO, LRU
* file: gcc.din, spice.din 
### Excute format 
執行格式: ```./cache cache_size block_size associativity "replace_policy" "file"```

## About output
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
![error]( https://images2017.cnblogs.com/blog/841412/201712/841412-20171218152527584-372058249.png "FlowChar")



