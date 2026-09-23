#include<iostream>
#include<cstdio>
#include<algorithm>
#include<windows.h>
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
char inputc()//限时输入
{
    DWORD on=GetTickCount();
    while(GetTickCount()<waittime+on)
    {
        if(_kbhit()) //检测按按键
            return getchar(); //读取
        Sleep(10);
    }
    return 0;
}
int inputn()
{
    string s;
    DWORD on=GetTickCount();
    while(GetTickCount()<waittime+on)
    {
        if(_kbhit()) //检测按按键
        {getline(cin,s);break;} //读取
        Sleep(10);
    }
    if(s.empty()) return 0;
    int n=atoi(s.c_str());//转换成数字
    return n;
}
/*void gotoxy(short x,short y)//移动光标
{
    COORD a={x,y};
    SetConsoleCursorPosition(handle_output,a);
}*/
bool o[777][77];
char get(short x,short y)//读取
{
    COORD pos={x,y};DWORD b;char c;
    ReadConsoleOutputCharacterA(handle_output,&c,1,pos,&b);
    return c;
}
void erase(short x,short y)//擦除
{
    if(o[x][y]) {o[x][y]=0;return;}
    COORD a={x,y};DWORD b;
    FillConsoleOutputCharacter(handle_output,' ',1,a,&b);
}
void output(short x,short y)//输出
{
    if(get(x,y)==c) o[x][y]=1;
    COORD a={x,y};DWORD b;
    FillConsoleOutputCharacter(handle_output,c,1,a,&b);
}
void move(letter&a)
{
         if(a.o==1) a.x++;
    else if(a.o==2) a.y++;
    else if(a.o==3) a.x--;
    else if(a.o==4) a.y--;
    for(;;)
             if(a.x>=W&&a.o==1) a.o=2;
        else if(a.y>=H&&a.o==2) a.o=3;
        else if(a.x<=0&&a.o==3) a.o=4;
        else if(a.y<=0&&a.o==4) a.o=1;
        else break;
}
void letter_went()
{
    for(;;)
    {
        move(Head);
        output(Head.x,Head.y);
        erase(End.x,End.y);
        move(End);
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
    H=0;//题目让我这么干的
    if(W==0)
    {printf("Your concole is too small!");return 0;}
    printf("Please input the number of letters within %d seconds.\n",waittime/1000);
    printf("If you do not input,it will be 7.\n");
    n=inputn();
    if(n<1 || n>W+H) n=7;
    printf("Please input the letter you want to run within %d seconds.\n",waittime/1000);
    printf("If you do not input,it will be '7'.\n\n");
    c=inputc();
    if(c<32 || c>126) c='7';
    printf("Don't change anything about the console window.\n");
    printf("When it is running,you can press the 'ESC' key to exit the program.\n");
    printf("Please press any key to start the game."); _getch();
    system("cls");
    End.x=End.y=Head.x=Head.y=0;End.o=Head.o=1;
    for(int i=1;i<n;i++)
    {
        move(Head);
        output(Head.x,Head.y);
    }
    output(0,0);
    letter_went();//开跑
    return 0;
}//其实还可以写定义颜色，但是我懒