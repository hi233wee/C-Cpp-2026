#include<cstdio>
#include<cstdlib>
#include<cctype>
#include<random>
#include<conio.h>
#include<Windows.h>
using namespace std;
typedef unsigned long long uLL;
typedef long long LL;
typedef unsigned int uint;
typedef unsigned short int usint;
#define Rand() RAND(RNG)

uLL rd()
{
    int c=getchar();uLL x=0;
    while(c!=EOF && !isdigit(c)) c=getchar();
    if(c==EOF) return 0;//ai说有神秘原因会输入这个
    while(isdigit(c))
    {x=x*10+c-48;c=getchar();}
    return x;
}
void wt(LL x)
{
    if(x<0){putchar(45);x=-x;}
    if(x>9) wt(x/10);
    putchar(x%10+48);
}

HANDLE handle_output=GetStdHandle(STD_OUTPUT_HANDLE);

void gotoxy(short x, short y);
int  get_cursor_line();
int  get_console_width();
void erase_line(int y);

struct linked_list
{
    LL V;//----------值
    linked_list* N;//下一项
};
uLL n;//-------------------------节点个数
LL L,R;//------------------------值域
char s[7777777],tmp[77];//-------输出缓冲
LL x;uint t,tt;//----------------辅助变量
void add(linked_list*& h, LL v);
void reverse(linked_list*& h);
void Free(linked_list*& h);
void putn(const linked_list* i);
void find_5(linked_list*& i, size_t& id);

int main() {
    system("cls");
    printf("请输入链表的长度\n  ");
    n=rd();
    if(n==0){
        printf("长度为零?");
        return 0;
    }
    printf("请输入链表中元素的取值范围\n");
    printf("   下界: ");L=rd();
    printf("   上界: ");R=rd();
    printf("好! 我将会输出随机生成的链表\n");
    printf("  然后把链表反转\n");

    random_device RD;
    mt19937 RNG(RD());
    uniform_int_distribution<LL> RAND(L,R);
    linked_list* head=nullptr;
    linked_list* it;//记录找到哪了
    //我的链表头存值
    for(size_t i=n+1;--i;) add(head,Rand());
    putn(head);
    reverse(head);

    printf("然后我会尝试找第一个5\n");
    printf("  并返回它的位置\n");
    it=head;
    size_t id=1;
    char c;
    for(;;) {
        find_5(it,id);
        printf("请按'ENTER'继续找下一个5, 或按'ESC'退出");
        for(;;) {
            c=_getch();
            if(c==27){
                erase_line(get_cursor_line());
                Free(head);
                return 0;
            }
            else if(c==13) break;
        }
        erase_line(get_cursor_line());gotoxy(0,get_cursor_line());
    }
    return 0;
}

//链表//

void add(linked_list*& h, LL v) {//---------------------添加(头插)
    linked_list* p=(linked_list*)malloc(sizeof(linked_list));
    p->N=h;
    p->V=v;
    h=p;
}

void reverse(linked_list*& h) {//-----------------------反转
    linked_list* i=nullptr;//下一个
    linked_list* p=nullptr;//上一个
                        //h//这一个
    i=h->N;
    while(i) {
        h->N=p;
        p=h;
        h=i;
        i=i->N;
    }
    h->N=p;
}

void Free(linked_list*& h) {//--------------------------释放空间
    linked_list* p;
    while(h->N) {
        p=h->N;
        free(h);
        h=p;
    }
    free(h);
    h=nullptr;
}

void putn(const linked_list* i) {//---------------------输出整个链表
    t=tt=0;
    while(i) {
        x=i->V;
        if(x<0) {
            tmp[tt++]=45;
            x=-x;
        }
        if(x==0) tmp[tt++]=48;
        while(x) {
            tmp[tt++]=x%10+48;
            x/=10;
        }
        while(tt) s[t++]=tmp[--tt];
        s[t++]='\n';
        if(t>7777700) {
            fwrite(s,1,t,stdout);//分块
            t=0;
        }
        i=i->N;
    }
    fwrite(s,1,t,stdout);
}

void find_5(linked_list*& i, size_t& id) {//------------找第一个值为5的节点
    while(i) {
        if(i->V==5) {
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

//控制台//

void gotoxy(short x, short y) {//-----------------------移动光标
    COORD a={x,y};
    SetConsoleCursorPosition(handle_output,a);
}

int get_cursor_line() {//-------------------------------获取光标行
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(handle_output,&a);
    return a.dwCursorPosition.Y;
}

int get_console_width() {//-----------------------------获取控制台宽度
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(handle_output,&a);
    return a.srWindow.Right-a.srWindow.Left+1;
}

void erase_line(int y) {//------------------------------擦除指定行
    COORD a;DWORD b;
    a.X=0;a.Y=y;
    FillConsoleOutputCharacter(handle_output,' ',get_console_width(),a,&b);
}
