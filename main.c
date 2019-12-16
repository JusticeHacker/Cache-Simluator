#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//argv[1] is (int)cache_size,8,16,...,256(KB)
//argv[2] is (int)block_size,4,8,....,128(Byte)
//argv[3] is (int)associativity,1,2,4,8,f(fully assoc.)
//argv[4] is (char*) replace-policy,FIFO or LRU
//argv[5] is (char*) file,gcc.din or spice.din
const char *hex_to_bin_quad(unsigned char c);
int main(int argc ,char**argv)
{
    int cacheSize,blockSize,replacePolicy,assoc;
    cacheSize=atoi(argv[1]);blockSize=atoi(argv[2]);

    if(strcmp("FIFO",argv[4])==0){
        replacePolicy=0;
    }else if(strcmp("LRU",argv[4])==0){
        replacePolicy=1;
    }
    if (strcmp(argv[3], "1")==0) {
        assoc = 1;
    }else if (strcmp(argv[3], "2")==0) {
        assoc = 2;
    }else if (strcmp(argv[3], "4")==0) {
        assoc = 4;
    }else if (strcmp(argv[3], "8")==0) {
        assoc = 8;
    }else if (strcmp(argv[3], "f")==0) {
        assoc = cacheSize * 1024 / blockSize;
    }
    //cacheSize=8;blockSize=32;assoc=1;replacePolicy=0;

    int blocks,offset,blockIdx,tag;

    blocks =(cacheSize*1024)/(blockSize*assoc);   //cache裡面總共有幾個櫃子
    blockIdx=log2(blocks);                       //這些櫃子需要blockIdx個位元去對應
    offset=log2(blockSize);
    tag=32-blockIdx-offset;                 //tag = 32bit扣掉用完剩下的那些bit
    //printf("blocks=%d,blockIdx=%d,tag=%d,assoc=%d\n",blocks,blockIdx,tag,assoc);
    /*  根據條件開出cache的大小  並把裡面初始化*/
    struct cache{
        char memoryAddress[32];
        int age;
        int dirty;
    }cache[blocks][assoc];//有blockIdx個block 每個block中可以放assoc個大小32位元的地址 跟age紀錄待了多久
    for(int i=0;i<blocks;i++)
    for(int j=0;j<assoc;j++)
    for(int k=0;k<32;k++)
    {
        cache[i][j].memoryAddress[k]=0;
        cache[i][j].age=0;
        cache[i][j].dirty=0;

    }


    FILE *input = fopen(argv[5], "r");
    char inputString[13];
    char memoryAddress[32],inputAddress[32];
    int inputLength,method,demandFetch=0,writeCount=0,readCount=0,hitCount=0,missCount=0,dirtyCount=0;
    int digitalAddress,cacheIdx,bytesToMem,bytesFromMem;
    while(fgets(inputString,13,input)){
        //printf("%s",inputString);
        memset(inputAddress, 0, 32);
        //printf("in\n");
        /*  計算demandFetch & read data & write data    */
        demandFetch++;  digitalAddress=0;
        method=inputString[0]-'0';  // 0 : data read,1 : data write,2 : instruction (read)
        if(method==0) readCount++;
        if(method==1) writeCount++;
        inputLength=strlen(inputString)-1;
        //printf("inputLength=%d\n",inputLength);
        if (inputString[0] == '2' || inputLength < 10) {
            strcat(inputAddress, "00000000");

        }
        /*  將16進位的地址先轉成2進位  */
        for(int i=2;i<inputLength;i++){
            strcat(inputAddress,hex_to_bin_quad(inputString[i]));    //inputAddress就是32位元的地址
        }
        //printf("%s\n",inputAddress);
        //system("pause");
        /* 將2進位的blockIdx轉成10進位  */
        int k=0;
        for(int i=tag;i<tag+blockIdx;i++){
            digitalAddress=digitalAddress+(inputAddress[i]-'0')*pow(2,k++);
            //printf("i=%d,digitalAddress=%d\n",i,digitalAddress);
        }
        //printf("digitalAddress=%d\n",digitalAddress);
        /* 將10進位數digitalAddress除以blocks得到要放在哪個櫃子 */
        cacheIdx=digitalAddress%blocks;
        //printf("cacheIdx=%d\n",cacheIdx);
        //system("pause");
        /*  判斷是hit 或是 miss          */
        int hitflag=0;
        for(int i=0;i<assoc;i++){
            if(strncmp(cache[cacheIdx][i].memoryAddress,inputAddress,tag)==0){
                hitflag=1;
                break;
            }
            if(i==assoc-1) hitflag=0;   //miss
        }
        /*             FIFO            */
        int max=-1;
        if(replacePolicy==0){
            if(hitflag==0)/*MISS*/{
                missCount++;
                for(int i=0;i<assoc;i++){
                    if(cache[cacheIdx][i].age>max) max=cache[cacheIdx][i].age;/*從cacheIdx中找最老的分支*/
                }
                for(int i=0;i<assoc;i++){
                    if(cache[cacheIdx][i].age==max){
                        strcpy(cache[cacheIdx][i].memoryAddress,inputAddress);
                        cache[cacheIdx][i].age=-1;
                        if(cache[cacheIdx][i].dirty==1) dirtyCount++;//他是髒的就先把它寫到mem
                        if(method==0||method==2) cache[cacheIdx][i].dirty=0;
                        if(method==1)            cache[cacheIdx][i].dirty=1;
                        break;//把最老的換掉並且讓更新它為最年輕的
                    }
                }
                for(int i=0;i<assoc;i++){
                    cache[cacheIdx][i].age++;   //更新每個人的年紀
                }
            }
            if(hitflag==1)/*hit*/{
                for(int i=0;i<assoc;i++){
                    if(strncmp(cache[cacheIdx][i].memoryAddress,inputAddress,tag)==0){
                        if(method==1) cache[cacheIdx][i].dirty=1;
                        break;
                    }
                }
                hitCount++;
            }
        }

        /*              LRU           */
        if(replacePolicy==1){
            if(hitflag==0)/*MISS*/{
                missCount++;
                for(int i=0;i<assoc;i++){
                    if(cache[cacheIdx][i].age>max) max=cache[cacheIdx][i].age;/*從cacheIdx中找最老的分支*/
                }
                for(int i=0;i<assoc;i++){
                    if(cache[cacheIdx][i].age==max){
                        strcpy(cache[cacheIdx][i].memoryAddress,inputAddress);
                        cache[cacheIdx][i].age=-1;
                        if(cache[cacheIdx][i].dirty==1) dirtyCount++;//他是髒的就先把它寫到mem
                        if(method==0||method==2) cache[cacheIdx][i].dirty=0;
                        if(method==1)            cache[cacheIdx][i].dirty=1;
                        break;//把最老的換掉並且讓更新它為最年輕的
                    }
                }
                for(int i=0;i<assoc;i++){
                    cache[cacheIdx][i].age++;   //更新每個人的年紀
                }
            }
            if(hitflag==1)/*hit*/{
                hitCount++;
                for(int i=0;i<assoc;i++){
                    if(strncmp(cache[cacheIdx][i].memoryAddress,inputAddress,tag)==0){
                        cache[cacheIdx][i].age=-1;  //LRU要把剛用過的快取視為最年輕的
                        if(method==1) cache[cacheIdx][i].dirty=1;
                        break;
                    }
                }
                for(int i=0;i<assoc;i++){
                    cache[cacheIdx][i].age++;   //更新每個人的年紀
                }
            }
        }
    }
    /*          clean up                */
    for(int i=0;i<blocks;i++)
    for(int j=0;j<assoc;j++)
    if(cache[i][j].dirty==1)    dirtyCount++;
    printf("Input file = %s\n", argv[5]);
    printf("Demand Fetch = %d\n", demandFetch);
    printf("Cache hit = %d\n", hitCount);
    printf("Cache miss = %d\n", missCount);
    printf("Miss rate = %.4f\n", ((double)missCount / (double)demandFetch));
    printf("Read data = %d\n", readCount);
    printf("Write data = %d\n", writeCount);
    printf("Bytes from memory = %d\n", blockSize*missCount);
    printf("Bytes to memory = %d\n", blockSize*dirtyCount);
}

const char *const quads[] = {  "0000", "0001", "0010", "0011", "0100", "0101",
    "0110", "0111", "1000", "1001", "1010", "1011",
    "1100", "1101", "1110", "1111"  };

const char *hex_to_bin_quad(unsigned char c)
{
    if (c >= '0' && c <= '9') return quads[     c - '0'];
    if (c >= 'A' && c <= 'F') return quads[10 + c - 'A'];
    if (c >= 'a' && c <= 'f') return quads[10 + c - 'a'];
    return NULL;
}

