#include<iostream>
#include<cstdio>
#include<algorithm>
#include<Windows.h>
#include<conio.h>
using namespace std;
#define waittime 7000
int rd()
{
    char c=getchar();int x=0;
    while(c!=EOF && !isdigit(c)) {c=getchar();}
    if(c==EOF) return 0;//ai说有神秘原因会输入这个
    while(isdigit(c))
    {x=x*10+c-'0';c=getchar();}
    return x;
}
void move(int n,char a,char b,char c)//把n个从a移到b
{
    if(n==1) {printf("%c -> %c\n",a,b);return;}
    move(n-1,a,c,b);
    printf("%c -> %c\n",a,b);//move(1,a,b,c);
    move(n-1,c,b,a);
}
int n;
int main()
{
    system("cls");
    printf("Please input the number of disks.");
    n=rd();
    if(n<1) return 0;
    move(n,'A','C','B');
    return 0;
}//O(2^n)这一块