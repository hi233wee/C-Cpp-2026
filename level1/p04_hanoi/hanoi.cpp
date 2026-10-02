#include<cstdio>
#include<cstdlib>
#include<cctype>
using namespace std;

#define waittime 7000
int rd ()
{
    int c=getchar (); int x=0;
    while (c!=EOF && !isdigit (c)) {c=getchar (); }
    if (c==EOF) return 0; //ai说有神秘原因会输入这个
    while (isdigit (c))
    {x=x*10+c-48; c=getchar (); }
    return x;
}

int n; //盘子数
void move (int n, char a, char b, char c);

int main () {
    system ("cls");
    printf ("请输入盘子的数量: ");
    n=rd ();
    if (n<1) return 0;
    move (n, 'A', 'C', 'B');
    return 0;
} //O(2^n)这一块

//汉诺塔//

void move (int n, char a, char b, char c) { //----------把n个盘子从塔a挪到塔b
    //把n个从a移到b, c作辅助//
    if (n==1) {printf ("%c -> %c\n", a, b); return; }
    move (n-1, a, c, b); //把上面n-1个挪走
    printf ("%c -> %c\n", a, b); //最下面的挪过去
    move (n-1, c, b, a); //n-1个挪过去
}
