#include<iostream>
#include<cstdio>
#include<algorithm>
#include<Windows.h>
#include<conio.h>
using namespace std;
#define waittime 7000
#define sleeptime 200
int getwidth()
{
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&a);
    return a.srWindow.Right-a.srWindow.Left+1;
}
int getheight()
{
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&a);
    return a.srWindow.Bottom-a.srWindow.Top+1;
}
char input()//限时输入
{
    DWORD on=GetTickCount();
    while(GetTickCount()<waittime+on)
    {
        if(_kbhit()) //检测按按键
            return _getch(); //读取
        Sleep(10);
    }
    return 0;
}
int n,c;
struct letter
{
    int x,y,o;//o为状态
};
void letterwent_output(int n,char c)
{
    int w=getwidth()-1,h=getheight()-1;//获取控制台窗口宽高
    letter *a=(letter*)malloc(sizeof(letter)*n);
    for(int i=0;i<n;i++)
        a[i].x=i,a[i].y=0,a[i].o=1;
    while(1)
    {
        system("cls");
        for(int i=0;i<n;i++)
        {
            if(a[i].o==1) a[i].x++;
            else if(a[i].o==2) a[i].y++;
            else if(a[i].o==3) a[i].x--;
            else if(a[i].o==4) a[i].y--;
            if(a[i].x==w) a[i].o=2;
            else if(a[i].y==h) a[i].o=3;
            else if(a[i].x==0) a[i].o=4;
            else if(a[i].y==0) a[i].o=1;
        }
        sort(a,a+n,[](letter a,letter b){return a.x<b.x?true:a.y<b.y?true:false;});
        for(int i=0;i<n;)
        {
            
        }
        Sleep(sleeptime);
    }
}
void GO()
{
    printf("Please input the number(1~9) of letters within %d seconds.If you do not input,it will be 7\n",waittime/1000);
    n=input();
    if(n==0) n=7;
    else n=n-'0';
    printf("Please input the letter you want to run within %d seconds.If you do not input,it will be '7'\n",waittime/1000);
    c=input();
    if(c==0) c='7';
    letterwent_output(n,c);
}
int main()
{
    GO();
    return 0;
}