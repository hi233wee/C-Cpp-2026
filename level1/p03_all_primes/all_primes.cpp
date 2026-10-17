#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <conio.h>
#include <windows.h>
using namespace std;

typedef unsigned int       uint ; // 32位
typedef unsigned short int usint; // 16位
typedef unsigned char      uchar; // 8位

const uint N = 1000; // 要计算的上界(最大10^9)
const uint M = N>>1;

#define TYPE uint //---------筛的类型
#define BITS 32 //-----------筛的类型的占用bit(位)
const uint NW= M/BITS+2; //--筛数组大小
TYPE o[NW]; //---------------位图筛子
// 每一位对应一个数
// 因为只筛奇数所以再除2

uchar T1[10]  ; //   0-9   //复制表
usint T2[100] ; //  00-99  //复制表
uint  T3[1000]; // 000-999 //复制表
// T4 有 40KB 内存到cpu远
void init_T () {
    char tmp[10]; // 临时字符(存一个数)
    for (int i=0; i<10  ; i++) { sprintf(tmp, "%01d", i); memcpy(T1+i, tmp, 1); }
    for (int i=0; i<100 ; i++) { sprintf(tmp, "%02d", i); memcpy(T2+i, tmp, 2); }
    for (int i=0; i<1000; i++) { sprintf(tmp, "%03d", i); memcpy(T3+i, tmp, 3); }
    // sprintf 把""打印到tmp (类似printf)// %0nd n位 不足左补0
    // 为什么补0: 固定位数 (会用到007)
    // memcpy 就是复制 最后那个是个数
}

char s[N*7]; //------------输出缓冲(反正*7能装)
uint i, j, w, k, x, t; //--i,k循环变量, x当前数, t缓冲长度
#define W1(x) { memcpy(s+t, &T1[x], 1); t+=1; }
#define W2(x) { memcpy(s+t, &T2[x], 2); t+=2; }
#define W3(x) { memcpy(s+t, &T3[x], 3); t+=3; }

inline void put(uint x) {
         if (x>999999) { W3(x/10000); W3(x/10%1000); W1(x%10); } // 7
    else if (x>99999 ) { W3(x/1000); W3(x%1000); }               // 6
    else if (x>9999  ) { W3(x/100); W2(x%100); }                 // 5
    else if (x>999   ) { W3(x/10); W1(x%10); }                   // 4
    else if (x>99    ) { W3(x); }                                // 3
    else if (x>9     ) { W2(x); }                                // 2
    else               { W1(x); }                                // 1
}

int main () {
    // 初始化 //
    SetConsoleOutputCP(65001); // UTF-8, 保证中文不乱码
    SetConsoleCP(65001);

    init_T(); // 打表总不能算我时间吧
    memset(o, 0, sizeof(o)); // 每一位填0, 0表示是质数
    o[0] |= 1u; // 去掉第一位

    system("cls");
    printf("这一句是没有什么意义的只是为了预热stdout防止fwrite时首次调用导致N较小的时候输出多至少0.1ms然后导致最终时间不好看\n");
    printf("顺带一提你的电脑被神秘程序占用内存和CPU时直接跑会慢建议丢给AI做预热加多次去极值取平均值\n");

    // 开始计时 //
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    // 埃氏筛 // 只筛奇数 x = i*2 +1
    s[0]='2'; s[1]='\n'; t=2;
    for (k=0; k<NW; ++k) {
        // 位运算更快 ( >> << | & ^ ! ~ )//但是优先级低
        w = ~o[k]; // 取反为了方便后面
        while (w) {
            // __builtin_ctz(i)返回低位连续0的个数
            i = (k<<5) + __builtin_ctz(w); // 全局下标
            if (i>=M) { w=0; break; }      // 后面全越界
            
            w &= ~(1u<<__builtin_ctz(w));  // 标记刚处理完的这一个
            x = (i<<1)|1;
            
            put(x);
            s[t++]='\n';

            for (j=i+x; j < M; j+=x) { o[j>>5] |= 1u<<(j&31); }
            w &= ~o[k]; // 丢掉本组刚被划掉的位
        }
    }
    // 一次输出, 其实能省很多时间 //
    fwrite(s, 1, t, stdout);
    // 停止计时 //
    QueryPerformanceCounter(&end);
    printf("花费时间:%.4lfms\n", (double)(end.QuadPart - start.QuadPart) /frequency.QuadPart *1000);

    // system("pause");
    return 0;
} // O(n log log n)

// 筛选端 //
// 位图, 位运算
// 当范围在10^9以上时, 还有几个优化, 但是我这小, 用了反而慢 (还有这几个难写)

// 输出端 //
// 一次性输出
// 复制表

// 硬件端 //
// 现在一般能一次处理32位(虽然但是我是x64)
// 现在一般当计算所需内存在32KB内, 可用最靠近CPU的那块缓存(只有4-5周期)

// 100000000000000000版(10^18)爆内存, 分段太恶心, 还有输出谁存得下