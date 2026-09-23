#include<iostream>
#include<cstdio>
#include<algorithm>
#include<Windows.h>
#include<conio.h>
using namespace std;
typedef unsigned short int usint;
void wt(usint x)
{
    if(x>9) wt(x/10);
    putchar(x%10+'0');
}
bool*o;
int main()
{
    system("cls");
    printf("I will output all prime numbers less than 1000,and the time it takes.\n");
    o=(bool*)malloc(sizeof(bool)*1007);
    memset(o,0,sizeof(o));
    LARGE_INTEGER frequency,start,end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    for(usint i=2,j;i<1000;i++)
    {
        if(!*(o+i)) wt(i),putchar('\n');
        for(j=i;j<1000;j+=i) *(o+j)=1;
    }
    QueryPerformanceCounter(&end);
    printf("used time:%.4lfms\n",(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000);
    return 0;
}//线性筛选  O(n)

//10000000版(10^7)

/*#include<iostream>
#include<cstdio>
#include<algorithm>
#include<Windows.h>
#include<conio.h>
using namespace std;
typedef unsigned int uint;
void wt(uint x)
{
    if(x>9) wt(x/10);
    putchar(x%10+'0');
}
bool*o;
int main()
{
    system("cls");
    printf("I will output all prime numbers less than 10000000,and the time it takes.\n");
    o=(bool*)malloc(sizeof(bool)*10000007);
    LARGE_INTEGER frequency,start,end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    for(uint i=2,j;i<10000000;i++)
    {
        if(!*(o+i)) wt(i),putchar('\n');
        for(j=i;j<10000000;j+=i) *(o+j)=1;
    }
    QueryPerformanceCounter(&end);
    printf("used time:%.4lfms\n",(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000);
    return 0;
}*/

//1000000000000000000版(10^18)

/*#include<iostream>
#include<cstdio>
#include<algorithm>
#include<Windows.h>
#include<conio.h>
using namespace std;
typedef unsigned long long uLL;
void wt(uLL x)
{
    if(x>9) wt(x/10);
    putchar(x%10+'0');
}
bool*o;
int main()
{
    system("cls");
    printf("I will output all prime numbers less than 1000000000000000000,and the time it takes.\n");
    o=(bool*)malloc(sizeof(bool)*1000000000000000007);
    LARGE_INTEGER frequency,start,end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    for(uLL i=2,j;i<1000000000000000000;j+=i)
    }
    QueryPerformanceCounter(&end);
    printf("used time:%.4lfms\n",(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000);
    return 0;
}//线性筛选  O(n)*/