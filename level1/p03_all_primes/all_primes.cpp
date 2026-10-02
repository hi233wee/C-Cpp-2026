
//1000版
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <conio.h>
#include <windows.h>
using namespace std;

typedef unsigned int uint;
typedef unsigned short int usint;

uint* o; //----------------------筛子: o 的第 i 位=1 表示 i 已被筛掉
char s[1000*7], tmp[7]; //-------输出缓冲
usint i, j, x, t=0, tt; //-------循环变量, t:缓冲长度 tt:单个数字的位数

int main () {
    //初始化//
    SetConsoleOutputCP(65001); //UTF-8, 保证中文不乱码
    SetConsoleCP(65001);

    system("cls");
    o = (uint*)calloc(1007/32+1, sizeof(uint)); //位图, 1007 位占 32 个 uint
    printf("  我将会输出所有小于1000的质数, 以及花费的时间\n");
    printf("按任意键继续...");
    //_getch();
    printf("\r               \r");//古法清行
    //开始计时//
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    //欧拉筛//
    for(i=2; i<1000; i++) {
        //是质数//
        if(!(o[i>>5] & (1u<<(i&31)))) {
            tt=0; x=i;
            while(x) {
                tmp[tt++] = x%10 +48; //+'0'
                x/=10;
            }
            while(tt) s[t++] =tmp[--tt];
            s[t++]='\n'; //插入换行
        }
        //把倍数标记//
        for(j=i; j<1000; j+=i) o[j>>5] |= 1u<<(j&31);
    }
    fwrite(s, 1, t, stdout);
    QueryPerformanceCounter(&end);
    printf("花费时间:%.4lfms\n", (double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000);
    return 0;
} //O(n)
//第二代输出, 一次性构造, 减少函数调用(还能这样)//我电脑约0.071ms

/*
//10000000版(10^7)
#include <iostream>
#include <cstdio>
#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <conio.h>
#include <windows.h>
using namespace std;
typedef unsigned int uint;
typedef unsigned short int usint;
uint* o; //----------------------筛子: o 的第 i 位=1 表示 i 已被筛掉
char s[10000000*8], tmp[7];
uint i, j, x; uint t=0, tt;
int main () {
    //初始化//
    SetConsoleOutputCP(65001); //UTF-8, 保证中文不乱码
    SetConsoleCP(65001);

    system("cls");
    printf("我将会输出所有小于10000000的质数, 以及花费的时间\n");
    o = (uint*)calloc(10000007/32+1, sizeof(uint));

    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    for(uint i=2, j; i<10000000; i++)
    {
        if(!(o[i>>5] & (1u<<(i&31))))
        {
            tt=0; x=i;
            while(x)
            {
                tmp[tt++] = x%10 +48; //+'0'
                x/=10;
            }
            while(tt)
                s[t++] =tmp[--tt];
            s[t++]='\n';
        }
        for(j=i; j<10000000; j+=i) o[j>>5] |= 1u<<(j&31);
    }
    fwrite(s, 1, t, stdout);
    QueryPerformanceCounter(&end);
    printf("花费时间:%.4lfms\n", (double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000);
    return 0;
} //O(n)
//第二代输出, 一次性构造, 减少函数调用(还能这样)//我电脑约1030ms


//100000000000000000版(10^18),爆内存, 不想写分段
*/