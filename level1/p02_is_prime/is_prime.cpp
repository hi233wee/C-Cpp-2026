#include<iostream>
#include<cstdio>
#include<algorithm>
#include<Windows.h>
#include<conio.h>
using namespace std;
typedef unsigned long long uLL;
HANDLE handle_output=GetStdHandle(STD_OUTPUT_HANDLE);
uLL rd()
{
    char c=getchar();uLL x=0;
    while(!isdigit(c)) c=getchar();
    while(isdigit(c))
    {x=x*10+c-'0';c=getchar();}
    return x;
}
void gotoxy(short x,short y)//移动光标
{
    COORD a={x,y};
    SetConsoleCursorPosition(handle_output,a);
}
int get_cursor_line()
{
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(handle_output,&a);
    return a.dwCursorPosition.Y;
}
int get_console_width()
{
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(handle_output,&a);
    return a.srWindow.Right-a.srWindow.Left+1;
}
void erase_line(int y)//擦除指定行
{
    COORD a;DWORD b;
    a.X=0;a.Y=y;
    FillConsoleOutputCharacter(handle_output,' ',get_console_width(),a,&b);
}
uLL base[]={2,3,5,7,11,13,17,19,23,29,31,37};
uLL multiply_mod(uLL a,uLL b,uLL mod)//a*b mod mod
{
    return (__int128)a*b%mod;
}
uLL power_mod(uLL a,uLL b,uLL mod)//a^b mod mod
{
    uLL x=1;
    while(b)
    {
        if(b&1) x=multiply_mod(x,a,mod);
        a=multiply_mod(a,a,mod);
        b>>=1;
    }
    return x;
}
bool is_prime(uLL n)
{
    if(n<2)    return false;
    if(n==2)   return true;
    if(n%2==0) return false;
    uLL d=n-1;int s=0;
    while(d%2==0) d>>=1,s++;
    for(int i=0;i<12;i++)
    {
        if(base[i]>=n) break;
        uLL x=power_mod(base[i],d,n);
        if(x==1||x==n-1) continue;
        bool o=false;
        for(int j=1;j<s;j++)
        {
            x=multiply_mod(x,x,n);
            if(x==n-1){o=true;break;}
        }
        if(!o) return false;
    }
    return true;
}
int main()
{
    system("cls");
    uLL n;char c;
    for(;;)
    {
        printf("Please input a number to check whether it is a prime number or not.\n");
        n=rd();
        erase_line(get_cursor_line()-1);erase_line(get_cursor_line()-2);gotoxy(0,get_cursor_line()-2);
        if(is_prime(n)) printf("%llu is a prime number.\n",n);
        else            printf("%llu is not a prime number.\n",n);
        printf("Please press 'ENTER' to continue or 'ESC' to exit.");
        for(;;)
        {
            c=_getch();
            if(c==27) {erase_line(get_cursor_line());return 0;}
            else if(c==13) break;
        }
        erase_line(get_cursor_line());gotoxy(0,get_cursor_line());
    }
    return 0;
}//米勒拉宾素性测试(n<2^64)