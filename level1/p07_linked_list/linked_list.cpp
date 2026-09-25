#include<iostream>
#include<cstdio>
#include<algorithm>
#include<ctime>
#include<utility>
#include<cstdlib>
#include<random>
#include<chrono>
#include<Windows.h>
#include<conio.h>
using namespace std;
typedef unsigned long long uLL;
typedef long long LL;
typedef unsigned short int usint;
#define Rand() RAND(RNG)
uLL rd()
{
    char c=getchar();uLL x=0;
    while(!isdigit(c)) c=getchar();
    while(isdigit(c))
    {x=x*10+c-'0';c=getchar();}
    return x;
}
void wt(LL x)
{
    if(x<0){putchar('-');x=-x;}
    if(x>9) wt(x/10);
    putchar(x%10+'0');
}
//=================控制台=================//
HANDLE handle_output=GetStdHandle(STD_OUTPUT_HANDLE);
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

struct linked_list
{
    LL V;          //Value
    linked_list* N;//Next
};
uLL n;
LL L,R;
void add(linked_list*& h,LL v)//添加
{
    linked_list* p=(linked_list*)malloc(sizeof(linked_list));
    p->N=h;
    p->V=v;
    h=p;
}
void reverse(linked_list*&h)//反向
{
    linked_list* i=nullptr;//下一个
    linked_list* p=nullptr;//上一个
                        //h//这一个
    i=h->N;
    while(i)
    {
        h->N=p;
        p=h;
        h=i;
        i=i->N;
    }
    h->N=p;
}
char s[777777777],tmp[20];
LL x;usint t,tt;
void putn(const linked_list* i)//输出
{
    t=tt=0;
    while(i)
    {
        x=i->V;
        while(x)
        {
            tmp[tt++]=x%10+'0';
            x/=10;
        }
        while(tt)
            s[++t]=tmp[--tt];
        s[++t]='\n';
        if(t>777777700)
        {
            fwrite(s,1,t+1,stdout);//分块
            t=0;
        }
        i=i->N;
    }
    fwrite(s,1,t+1,stdout);
}
void find_5(linked_list* &i,size_t &id)
{
    while(i)
    {
        if(i->V==5)
        {
            printf("%llu\n",uLL(id));
            i=i->N;
            id++;
            return;
        }
        i=i->N;
        id++;
    }
    fputs("-1\n",stdout);
}
int main()
{
    system("cls");
    printf("Please input the size of the linked list\n  ");
    n=rd();
    printf("Please input the value range of elements in the linked list\n");
    printf("   left bound: ");L=rd();
    printf("  right bound: ");R=rd();
    printf("Right! I will output the the randomly generated linked list\n");
    printf("  Then reverse the linked list\n");

    random_device RD;
    mt19937 RNG(RD());
    uniform_int_distribution<LL> RAND(L,R);
    linked_list* head=(linked_list*)malloc(sizeof(linked_list));
    linked_list* it  =(linked_list*)malloc(sizeof(linked_list));//记录找到哪了
    head->N=nullptr;
    head->V=Rand();
    //我的链表头存值
    for(size_t i=n;--i;) add(head,Rand());//去掉第一个
    putn(head);
    reverse(head);
    
    printf("  Then try to find the first 5.\n");
    printf("  And return the id of it.\n");
    it=head;
    size_t id=1;
    char c;
    for(;;)
    {
        find_5(it,id);
        printf("Please press 'ENTER' to find the next 5 or 'ESC' to exit.");
        for(;;)
        {
            c=_getch();
            if(c==27) {erase_line(get_cursor_line());return 0;}
            else if(c==13) break;
        }
        erase_line(get_cursor_line());gotoxy(0,get_cursor_line());
    }
    return 0;
}