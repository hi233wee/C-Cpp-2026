#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <windows.h>
using namespace std;

#define waittime 7000

int rd ();

int n; //盘子数
int t=-1; //辅助下标
char s[7777777]; //输出缓冲
void add (const char& a, const char& b);
void move (int n, char a, char b, char c);

int main () {
    //初始化//
    SetConsoleOutputCP(65001); //UTF-8, 保证中文不乱码
    SetConsoleCP(65001);

    system("cls");
    printf("请输入盘子的数量: ");
    n=rd();
    if(n<1) return 0;

    move(n, 'A', 'C', 'B');
    fwrite(s, 1, t, stdout);
    return 0;
} //O(2^n)这一块

//汉诺塔//

void move (int n, char a, char b, char c) { //----------把n个盘子从塔a挪到塔b
    //把n个从a移到b, c作辅助//
    if(n==1) { add(a, b); return; }
    move(n-1, a, c, b); //把上面n-1个挪走
    add(a, b); //最下面的挪过去
    move(n-1, c, b, a); //n-1个挪过去
}

void add (const char& a, const char& b) { //------------加入缓冲
    s[++t]=a;
    s[++t]=' ';
    s[++t]='-';
    s[++t]='>';
    s[++t]=b;
    s[++t]='\n';
    if(t>7777700) {
        fwrite(s, 1, t, stdout);
        t=-1;
    }
}

int rd () {
    int c=getchar(); int x=0;
    while(c!=EOF && !isdigit(c)) c=getchar();
    if(c==EOF) return 0; //ai说有神秘原因会输入这个
    while(isdigit(c))
    { x = x*10 +c -48; c=getchar(); }
    return x;
}
