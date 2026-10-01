//1000版
#include<cstdio>
#include<cstdlib>
#include<cstring>
#include<Windows.h>
using namespace std;

typedef unsigned short int usint;

bool* o;//-----------------------筛子: o[i]=1 表示 i 已被筛掉
char s[1000*7],tmp[7];//---------输出缓冲
usint i,j,x;usint t=0,tt;//------循环变量, t:缓冲长度 tt:单个数字的位数

int main() {
    system("cls");
    printf("我将会输出所有小于1000的质数, 以及花费的时间\n");
    o=(bool*)malloc(sizeof(bool)*1007);
    memset(o,0,sizeof(bool)*1007);

    LARGE_INTEGER frequency,start,end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    for(i=2;i<1000;i++) {
        if(!*(o+i)) {
            tt=0;
            x=i;
            while(x) {
                tmp[tt++]=x%10+'0';
                x/=10;
            }
            while(tt) s[t++]=tmp[--tt];
            s[t++]='\n';
        }
        for(j=i;j<1000;j+=i) *(o+j)=1;
    }
    fwrite(s,1,t,stdout);
    QueryPerformanceCounter(&end);
    printf("花费时间:%.4lfms\n",(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000);
    return 0;
}//线性筛选  O(n)
//第二代输出，一次性构造，减少函数调用（还能这样）//约0.075ms

/*
//10000000版(10^7)
#include<iostream>
#include<cstdio>
#include<algorithm>
#include<Windows.h>
#include<conio.h>
using namespace std;
typedef unsigned int uint;
typedef unsigned short int usint;
bool* o;
char s[10000000*8],tmp[7];
uint i,j,x;uint t=0,tt;
int main()
{
    system("cls");
    printf("我将会输出所有小于10000000的质数, 以及花费的时间\n");
    o=(bool*)malloc(sizeof(bool)*10000007);
    memset(o,0,sizeof(bool)*10000007);

    LARGE_INTEGER frequency,start,end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    for(uint i=2,j;i<10000000;i++)
    {
        if(!*(o+i))
        {
            tt=0;
            x=i;
            while(x)
            {
                tmp[tt++]=x%10+'0';
                x/=10;
            }
            while(tt)
                s[t++]=tmp[--tt];
            s[t++]='\n';
        }
        for(j=i;j<10000000;j+=i) *(o+j)=1;
    }
    fwrite(s,1,t,stdout);
    QueryPerformanceCounter(&end);
    printf("花费时间:%.4lfms\n",(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000);
    return 0;
}
//第二代输出，一次性构造，减少函数调用（还能这样）//约1100ms
*/

//100000000000000000版(10^18)   爆内存，不想写分段
