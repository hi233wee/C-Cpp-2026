#include<iostream>
#include<vector>
#include<stack>
#include<string>
#include<utility>
#include<cstdlib>
#include<ctime>
#include<random>
#include<chrono>
#include<cstdio>
#include<conio.h>
#include<windows.h>
using namespace std;

/* ==================== 随机函数 ====================*/

random_device RD;
mt19937 RNG(RD());
uniform_int_distribution<int> RAND(0,11);
inline int Rand()
{
    return RAND(RNG);
}
uniform_int_distribution<int> RAND_COLOR(1,15);//1-15随机颜色,避开黑色(通关画面背景是黑的)
inline int Rand_color()
{
    return RAND_COLOR(RNG);
}

/* ==================== 控制台工具函数 ==================== */

HANDLE handle_output=GetStdHandle(STD_OUTPUT_HANDLE);
void hide_cursor()//---------------------------------隐藏光标
{
    CONSOLE_CURSOR_INFO a;
    a.bVisible=0;
    a.dwSize=1;
    SetConsoleCursorInfo(handle_output,&a);
}
void gotoxy(short x,short y)//-----------------------移动光标
{
    COORD a={x,y};
    SetConsoleCursorPosition(handle_output,a);
}
void get_console_size(int& w,int& h)//-----------------获取控制台宽高
{
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(handle_output,&a);
    w=a.srWindow.Right-a.srWindow.Left+1;
    h=a.srWindow.Bottom-a.srWindow.Top+1;
}
void set_color(int BG,int FG)//---------------------设置颜色；BG背景；FG前景
{
    SetConsoleTextAttribute(handle_output,(WORD)((BG<<4)|FG));
}
const char* COLOR_NAMES[16]=//-----------------------颜色模版
{"黑色","深蓝","深绿","青色","深红","紫色","暗黄","浅灰","深灰","亮蓝","亮绿","亮青","亮红","亮紫","黄色","白色"};
/* 0      1      2      3     4      5      6      7     8      9     10     11     12     13    14     15  */

/* ==================== 界面 ==================== */

void start_screem()//--------------------------------开始界面
{
    set_color(0,10);printf("=================================================\n");
                    printf("                 迷 宫 小 游 戏\n");
                    printf("=================================================\n");
    set_color(0,12);printf("[操作引导]\n");
    set_color(0,7 );printf("  ↑↓←→  w/s/a/d ：移动\n");
                    printf("  ESC / Q       ：退出游戏\n");
                    printf("  绿色的 ");
    set_color(0,10);putchar('S');
    set_color(0,7 );printf(" 是入口(起点)，红色的 ");
    set_color(0,12);putchar('E');
    set_color(0,7 );printf(" 是出口(终点)\n");
    set_color(0,11);printf("-------------------------------------------------\n");
    set_color(0,7 );printf("  接下来自定义游戏外观：\n\n");
}
void end_screen(double ms)//-------------------------通关画面
{
    set_color(0,7);//先恢复默认颜色再清屏，防止背景残留游戏配色
    system("cls");
    int m=ms/60000;
    double s=(ms-m*60000)/1000.0;

    set_color(0,10);printf("\n================================================\n");
    set_color(0,14);printf("   * * *  ");
    set_color(0,12);printf("恭 喜 你 成 功 走 出 迷 宫 !");
    set_color(0,14);printf("  * * *\n");
    set_color(0,10);printf("================================================\n\n");
    set_color(0,14);printf("          *    *    *    *    *    *\n");
                    printf("        *      Y O U   W I N ! !     *\n");
                    printf("          *    *    *    *    *    *\n\n");
    set_color(0,7 );printf("           通关总耗时： ");
    if(m>0)
    {set_color(0,11);printf("%d 分 %.4lf 秒\n",m,s);}
    else
    {set_color(0,11);printf("%.4lf 秒\n",s);}
    set_color(0,7 );printf("\n  按任意键继续……");
    while(!_kbhit())
    {
        set_color(0,Rand_color());
        gotoxy(15,6);//"Y O U   W I N ! !"的位置
        printf("Y O U   W I N ! !");
        Sleep(150);
    }
    _getch();
    gotoxy(0,11);//光标返回
}

/* ==================== 选择 ==================== */

char pick_symbol()//---------------------------------选角
{
    for(;;)
    {
        set_color(0,7);
        printf("请输入一个字符作为玩家符号(你看得见就行)。(回车默认 7): ");
        fflush(stdout);
        string s;
        getline(cin,s);
        if (s.empty()) return '7';
        for(int i=0;i<s.size();i++)
        {
            unsigned char c=(unsigned char)s[i];
            if(c>32&&c<127) return (char)c;//取第一个可见 ASCII 字符
        }
        set_color(0,7 );printf("你……我……叫你输 ");
        set_color(0,12);printf("一个！可见的！");
    }
}
void show_color_table()//----------------------------展示颜色选择
{
    for (int i=0;i<16;i++)
    {
        set_color(0,7);printf("  %2d:",i);
        set_color(0,i);printf("%s     ",COLOR_NAMES[i]);
        if(i%4==3)putchar('\n');
    }
}
int pick_color(const string& title, int DEf_COlOR)//-选颜色
{
    for(;;)
    {
        set_color(0,7        );printf("-------请选择%s(0-15,回车默认%d:",title.c_str(),DEf_COlOR);
        set_color(0,DEf_COlOR);printf("%s",COLOR_NAMES[DEf_COlOR]);
        set_color(0,7        );printf(")-------\n");
        show_color_table();
        printf("输入编号: ");
        fflush(stdout);
        string s;
        getline(cin,s);
        if (s.empty()) return DEf_COlOR;
        int n=atoi(s.c_str());//转换成数字
        if(n>=0&&n<=15) return n;
        set_color(0,7 );printf("你瞎吗? ");
        set_color(0,12);printf("0-15");
        set_color(0,7 );printf("看不见?\n");
    }
}

/* ==================== 生成迷宫 ==================== */

void generate_maze(vector<string>& maze,int C_ROOM,int R_ROOM)
{
    int GW=2*C_ROOM+3;
    int GH=2*R_ROOM+3;//网格比(2n+1)再多一圈
    vector<vector<bool> > vis(R_ROOM,vector<bool>(C_ROOM,0));//标记
        vis[0][0]=1;
    stack<pair<int,int> > st;//这两个存的都是房间
        st.push(make_pair(0,0));
    maze.assign(GH,string(GW,'#'));
    maze[2][2]=' ';

    int R,C,NR,NC,n,cand[4],d;//cand 存候选挖墙方向
    const int DR[4]={-1, 1, 0, 0};
    const int DC[4]={ 0, 0,-1, 1};//偏移
    while(!st.empty())//DFS 回溯法，保证全联通
    {
        R=st.top().first;
        C=st.top().second;
        n=0;
        for (int i=0;i<4;i++)
        {
            NR=R+DR[i];NC=C+DC[i];
            if (NR>=0 && NR<R_ROOM && NC>=0 && NC<C_ROOM && !vis[NR][NC])
                cand[n++]=i;
        }
        if(n==0) {st.pop();continue;}

        d=cand[Rand()%n];
        NR=R+DR[d];NC=C+DC[d];
        maze[2*R+2+DR[d]][2*C+2+DC[d]]=' ';//挖墙
        maze[2*NR+2][2*NC+2]=' ';
        vis[NR][NC]=1;
        st.push(make_pair(NR, NC));//新房间
    }

    for (int x=0;x<GW;x++) {maze[0][x]='7';maze[GH-1][x]='7';}
    for (int y=0;y<GH;y++) {maze[y][0]='7';maze[y][GW-1]='7';}//墙外留一圈隐藏捷径

    maze[1][2]      ='S';//入口
    maze[GH-2][GW-3]='E';//出口
}

/* ==================== 绘制迷宫 ==================== */

void draw_maze(const vector<string>& maze,int WALL_COLOR,int BG_COLOR)
{
    int GH=maze.size();
    int GW=maze[0].size();
    char c;
    for(int y=0;y<GH;y++)
    {
        gotoxy(0,y);
        for(int x=0;x<GW;x++)
        {
            c=maze[y][x];
                 if(c=='#') {set_color(BG_COLOR,WALL_COLOR);fputs("█",stdout);}
            else if(c=='7') {set_color(BG_COLOR,WALL_COLOR);fputs("▒",stdout);}
            else if(c==' ') {set_color(BG_COLOR,BG_COLOR  );putchar(' ');     }
            else if(c=='S') {set_color(BG_COLOR,10        );putchar('S');     }
            else            {set_color(BG_COLOR,12        );putchar('E');     }
        }
    }
}

/* ==================== 运行游戏 ==================== */

void run_game(const vector<string>& maze,char PLAYER,int PLAYER_COLOR,int WALL_COLOR,int BG_COLOR)
{
    int GH=maze.size();
    int GW=maze[0].size();
    int px=2,py=1;
    gotoxy(px,py);
    set_color(BG_COLOR,PLAYER_COLOR);
    putchar(PLAYER);//玩家放到入口

    LARGE_INTEGER frequency,start,end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);//开始计时
    
    int key,dx,dy,k,nx,ny;
    for(;;)//主循环
    {
        key=_getch();
        if(key==27 || key=='q' || key=='Q')//随时退出
        {
            set_color(0,7);
            system("cls");
            printf("  是不是不会玩?\n");
            return;
        }

        dx=0,dy=0;
        if(key==0 || key==224)// 功能键 / 方向键
        {
            k=_getch();
                 if(k==72) dy=-1;// ↑
            else if(k==80) dy= 1;// ↓
            else if(k==75) dx=-1;// ←
            else if(k==77) dx= 1;// →
        }
        else if(key=='w' || key=='W') dy=-1;
        else if(key=='s' || key=='S') dy= 1;
        else if(key=='a' || key=='A') dx=-1;
        else if(key=='d' || key=='D') dx= 1;
        if(dx==0 && dy==0) continue;

        nx=px+dx;ny=py+dy;
        if (nx<0 || ny<0 || nx>=GW || ny>=GH) continue;
        if (maze[ny][nx]=='#') continue;//撞墙

        gotoxy(px,py);
             if(maze[py][px]=='S') {set_color(BG_COLOR,10        );putchar('S');     }
        else if(maze[py][px]=='7') {set_color(BG_COLOR,WALL_COLOR);fputs("▒",stdout);}
        else                       {set_color(BG_COLOR,BG_COLOR  );putchar(' ');     }

        px=nx;py=ny;
        if(maze[py][px]=='E')
        {
            QueryPerformanceCounter(&end);//结束计时
            double sec=(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000;
            end_screen(sec);
            return;
        }

        gotoxy(px,py);
        set_color(BG_COLOR,PLAYER_COLOR);
        putchar(PLAYER);//玩家移动
    }
}

/* ==================== 主函数 ==================== */

int main()
{
    /* ---------- 初始设置 ---------- */
    SetConsoleOutputCP(65001);//UTF-8，保证中文不乱码
    SetConsoleCP(65001);
    hide_cursor();
    system("cls");

    for(;;)
    {
        start_screem();

        /* ---------- 自定义：符号 / 颜色 ---------- */
        char PLAYER      =pick_symbol();
        int  PLAYER_COLOR=pick_color("玩家的颜色",10);
        int  WALL_COLOR  =pick_color("墙壁的颜色",15);
        int  BG_COLOR    =pick_color("背景的颜色", 0);

        /* ---------- 下面这行能进去真的是…… ---------- */
        if(PLAYER_COLOR==BG_COLOR || WALL_COLOR==BG_COLOR)
        {
            set_color(0,7);
            printf("  你自己选的啊，看不见别怪我");
            Sleep(7000);
        }
        
        /* ---------- 迷宫大小由控制台窗口决定 ---------- */
        int W,H;
        get_console_size(W,H);
        int R_ROOM=(H-6)/2;//房间行列数
        int C_ROOM=(W-4)/2;//预留: 外圈捷径 + 底部引导行
        if(R_ROOM<2 || C_ROOM<2)
        {
            set_color(0,12);
            printf("\n  这么小的窗口你玩啥呢\n");
            set_color(0,7);
            system("pause");
            return 0;
        }

        vector<string> maze;
        generate_maze(maze,C_ROOM,R_ROOM);

        system("cls");
        draw_maze(maze,WALL_COLOR,BG_COLOR);

        /* ---------- 底部简略引导 ---------- */
        gotoxy(0,maze.size()+1);
        set_color(BG_COLOR,WALL_COLOR);
        printf("  ↑↓←→ w/s/a/d 移动 | ESC/Q 退出 | S 入口  E 出口 ");

        run_game(maze,PLAYER,PLAYER_COLOR,WALL_COLOR,BG_COLOR);

        /* ---------- 游戏结束提示 ---------- */
        set_color(0,11);printf("-------------------------------------------------\n");
        set_color(0,7 );printf("  按 ESC 或 Q 退出游戏，按任意键返回主菜单\n");
        fflush(stdout);
        int key=_getch();
        if(key==27 || key=='q' || key=='Q') break;
    }
}//随机迷宫+一堆功能（AI干的，我学习学习）
/*
学习成果
1.static +
2.const + 
3.获取控制台宽高可以写一起
4.注释也可以这么美丽
5.对齐可视化
6.{"","",……}依次返回存储""字符常量的首地址
7.&引用，本质也是指针
8.for(;;)
9.fflush(stdout)更新画面
10.unsigned char
11.string(n,ch)生成
12.超强随机函数
*/