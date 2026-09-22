#include<iostream>
#include<cstdio>
#include<algorithm>
#include<Windows.h>
#include<conio.h>
using namespace std;
#define waittime 7000
#define sleeptime 50
int n,W,H;char c;
struct letter
{
    int x,y,o;//o为状态
}Head,End;
HANDLE handle_output=GetStdHandle(STD_OUTPUT_HANDLE);
void hide_cursor()//隐藏光标
{
    CONSOLE_CURSOR_INFO a;
    a.bVisible=0;
    a.dwSize=1;
    SetConsoleCursorInfo(handle_output,&a);
}
void get_console_size(int&w,int&h)//获取控制台宽高
{
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&a);
    w=a.srWindow.Right-a.srWindow.Left+1;
    h=a.srWindow.Bottom-a.srWindow.Top+1;
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
/*void gotoxy(short x,short y)//移动光标
{
    COORD a={x,y};
    SetConsoleCursorPosition(handle_output,a);
}*/
void erase_output(int x_1,int y_1,int x_2,int y_2)//擦除输出
{
    COORD a;DWORD b;
    a.X=x_1;a.Y=y_1;
    FillConsoleOutputCharacter(handle_output,' ',1,a,&b);
    a.X=x_2;a.Y=y_2;
    FillConsoleOutputCharacter(handle_output,c,1,a,&b);
}
void letter_went()
{
    for(;;)
    {
             if(Head.o==1) Head.x++;
        else if(Head.o==2) Head.y++;
        else if(Head.o==3) Head.x--;
        else if(Head.o==4) Head.y--;
             if(Head.x==W&&Head.o==1) Head.o=2;
        else if(Head.y==H&&Head.o==2) Head.o=3;
        else if(Head.x==0&&Head.o==3) Head.o=4;
        else if(Head.y==0&&Head.o==4) Head.o=1;
        erase_output(End.x,End.y,Head.x,Head.y);
             if(End.o==1) End.x++;
        else if(End.o==2) End.y++;
        else if(End.o==3) End.x--;
        else if(End.o==4) End.y--;
             if(End.x==W&&End.o==1) End.o=2;
        else if(End.y==H&&End.o==2) End.o=3;
        else if(End.x==0&&End.o==3) End.o=4;
        else if(End.y==0&&End.o==4) End.o=1;
        Sleep(sleeptime);
        if(_kbhit()) if(_getch()==27) break;
    }
}
int main()
{
    system("cls");
    hide_cursor();
    get_console_size(W,H);//获取控制台窗口宽高
    W--;H--;
    printf("Please input the number(1~9) of letters within %d seconds.",waittime/1000);
    printf("If you do not input,it will be 7.\n");
    n=input();
    if(n==0) n=7;
    else n=n-'0';
    printf("Please input the letter you want to run within %d seconds.",waittime/1000);
    printf("If you do not input,it will be '7'.\n");
    c=input();
    if(c==0) c='7';
    printf("Don't change anything about the console window.\n");
    printf("When it is running,you can press the 'ESC' key to exit the program.\n");
    printf("Please press any key to start the game."); _getch();
    system("cls");
    End.x=End.y=Head.x=Head.y=0;End.o=Head.o=1;
    for(int i=1;i<n;i++)
    {
             if(Head.o==1) Head.x++;
        else if(Head.o==2) Head.y++;
        else if(Head.o==3) Head.x--;
        else if(Head.o==4) Head.y--;
             if(Head.x==W&&Head.o==1) Head.o=2;
        else if(Head.y==H&&Head.o==2) Head.o=3;
        else if(Head.x==0&&Head.o==3) Head.o=4;
        else if(Head.y==0&&Head.o==4) Head.o=1;
        erase_output(0,0,Head.x,Head.y);
    }
    erase_output(0,0,0,0);
    letter_went();//开跑
    return 0;
}//其实还可以写定义颜色，但是我懒