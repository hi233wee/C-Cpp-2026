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
#include<fstream>
#include<set>
#include<map>
using namespace std;

/* ==================== 神秘插件 ====================*/

/*void wait(int ms)//--------------------------------等待
{
    DWORD on=GetTickCount();
    for(;;)
    {
        if(GetTickCount()>ms+on)
        {
            break;
        }
        if(_kbhit()) _getch();
    }
}*/

/* ==================== 随机函数 ====================*/

random_device RD;
mt19937 RNG(RD());
uniform_int_distribution<int> RAND_COLOR(1,15);//1-15随机颜色,避开黑色
inline int Rand_color()
{
    return RAND_COLOR(RNG);
}

/* ==================== 控制台工具 ==================== */

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
void get_console_size(int& w,int& h)//---------------获取控制台宽高
{
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(handle_output,&a);
    w=a.srWindow.Right-a.srWindow.Left+1;
    h=a.srWindow.Bottom-a.srWindow.Top+1;
}
void set_color(int BG,int FG)//----------------------设置颜色；BG背景；FG前景
{
    SetConsoleTextAttribute(handle_output,(WORD)((BG<<4)|FG));
}
const char* COLOR_NAMES[16]=//-----------------------颜色模版
{"黑色","深蓝","深绿","青色","深红","紫色","暗黄","浅灰","深灰","亮蓝","亮绿","亮青","亮红","亮紫","黄色","白色"};
/* 0      1      2      3     4      5      6      7     8      9     10     11     12     13    14     15  */

/* ==================== 游戏数据 ==================== */

string DISP[256];//----------------------------------字符对应表:地图字符->显示字符
set<pair<int,int> > BOX,TARGET;//--------------------箱子/目标点坐标
map<string,int> SCORE_BEST;//------------------------每关历史最佳步数
const char* LEVEL_DIR="levels";//--------------------关卡子文件夹

string base_name(const string& s)//------------------去掉路径,只留文件名
{
    size_t p=s.find_last_of("\\/");
    return p==string::npos?s:s.substr(p+1);
}

void init_disp()//-----------------------------------恢复默认对应表
{
    for(int i=0;i<256;i++) DISP[i]=string(1,(char)i);
    DISP['0']=" ";
    DISP[' ']=" ";
}

/* ==================== 界面 ==================== */

void start_screem()//--------------------------------开始界面
{
    system("cls");
    set_color(0,10);printf("=================================================\n");
                    printf("                     推 箱 子\n");
                    printf("=================================================\n");
    set_color(0,12);printf("[操作引导]\n");
    set_color(0,7 );printf("  ↑↓←→  w/s/a/d ：移动\n");
                    printf("  ESC / Q       ：退出游戏\n");
                    printf("  把箱子全部推到目标点上就过关\n");
    set_color(0,12);printf("[温馨提醒]\n");
    set_color(0,7 );printf("  你的控制台尽量大点，不然……\n");
    set_color(0,11);printf("-------------------------------------------------\n");
    set_color(0,7 );printf("  按任意键继续……\n\n");
    _getch();
}
void end_screen(double ms,int STEP,int BEST,bool NEW_RECORD)//通关画面(带吐槽)
{
    set_color(0,7);//先恢复默认颜色再清屏，防止背景残留游戏配色
    system("cls");
    int m=ms/60000;
    double s=(ms-m*60000)/1000.0;

    set_color(0,10);printf("\n================================================\n");
    set_color(0,14);printf("            * * *  ");
    set_color(0,12);printf("你 过 关 !");
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
    set_color(0,7 );printf("           消耗步数： ");
    set_color(0,11);printf("%d 步\n",STEP);
    set_color(0,7 );printf("           历史最佳： ");
    if(NEW_RECORD)
    {set_color(0,10);printf("%d 步   新纪录！裱起来！\n",STEP);}
    else
    {set_color(0,11);printf("%d 步\n",BEST);}
    set_color(0,13);
         if(STEP<=10 )printf("           吐槽：你开了吧？\n");
    else if(STEP<=30 )printf("           吐槽：有点东西。\n");
    else if(STEP<=60 )printf("           吐槽：中规中矩，还能更快。\n");
    else if(STEP<=100)printf("           吐槽：箱子都替你着急。\n");
    else              printf("           吐槽：地板都被你磨出火星子了。\n");
    set_color(0,7 );printf("  按 ENTER 键继续……");
    for(;;)
    {
        set_color(0,Rand_color());
        gotoxy(15,6);//"Y O U   W I N ! !"的位置
        printf("Y O U   W I N ! !");
        Sleep(150);
        if(_kbhit()) if(_getch()=='\r') break;
    }
    gotoxy(0,14);//光标返回
}
int level_no(const string& s)//----------------------从文件名里抠出关卡号
{
    int n=0;
    for(size_t i=0;i<s.size();i++)
        if(s[i]>='0' && s[i]<='9') n=n*10+s[i]-'0';
    return n;
}
string choose_level()//------------------------------选关界面
{
    /* ---------- 扫描 levels 子文件夹下的 level*.push_boxes ---------- */
    vector<string> LEVELS;
    WIN32_FIND_DATAA a;
    HANDLE HF=FindFirstFileA((string(LEVEL_DIR)+"\\level*.push_boxes").c_str(),&a);
    if(HF!=INVALID_HANDLE_VALUE)
    {
        do{LEVELS.push_back(a.cFileName);}while(FindNextFileA(HF,&a));
        FindClose(HF);
    }
    /* 按关卡号排序,不然 10 会排到 level2 前面 */
    for(size_t i=0;i<LEVELS.size();i++)
        for(size_t j=i+1;j<LEVELS.size();j++)
            if(level_no(LEVELS[i])>level_no(LEVELS[j])) swap(LEVELS[i],LEVELS[j]);

    set_color(0,7);
    system("cls");
    set_color(0,10);printf("================================================\n");
                    printf("                    选 关 卡\n");
                    printf("================================================\n");
    if(LEVELS.empty())
    {
        set_color(0,12);printf("  我关卡文件呢?\n");
        set_color(0,7 );printf("  %s里怎么啥也没有?\n",LEVEL_DIR);
        system("pause");
        return "";
    }
    for(size_t i=0;i<LEVELS.size();i++)
    {
        set_color(0,14);printf("  %2d. ",(int)i+1);
        set_color(0,7 );printf("%-24s",LEVELS[i].c_str());
        map<string,int>::iterator it=SCORE_BEST.find(LEVELS[i]);
        if(it!=SCORE_BEST.end())
        {set_color(0,10);printf("  最佳：%d 步",it->second);}
        else
        {set_color(0,8 );printf("  还没人征服过");}
        putchar('\n');
    }
    set_color(0,11);printf("-------------------------------------------------\n");
    set_color(0,7 );printf("  输入编号选关，输 0 退出游戏： ");
    fflush(stdout);
    string s;
    getline(cin,s);
    int n=atoi(s.c_str());
    if(n>=1 && n<=(int)LEVELS.size()) return string(LEVEL_DIR)+"\\"+LEVELS[n-1];
    return "";
}

/* ==================== 游戏存取 ==================== */

void load_scores()//----------------------------------读历史最佳
{
    SCORE_BEST.clear();
    ifstream in("score.push_boxes");
    string name;
    int step;
    while(in>>name>>step) SCORE_BEST[name]=step;
}
bool save_score(const string& LEVEL_NAME,int STEP)//--破纪录才更新,返回是否新纪录
{
    map<string,int>::iterator it=SCORE_BEST.find(LEVEL_NAME);
    if(it!=SCORE_BEST.end() && it->second<=STEP) return 0;
    SCORE_BEST[LEVEL_NAME]=STEP;
    ofstream out("score.push_boxes");
    for(it=SCORE_BEST.begin();it!=SCORE_BEST.end();it++)
        out<<it->first<<" "<<it->second<<"\n";
    return 1;
}
bool load_level(const string& LEVEL_NAME,vector<string>& MAP,int& px,int& py)//读关卡文件
{
    ifstream in(LEVEL_NAME.c_str());
    if(!in.is_open()) return 0;

    init_disp();//每关先恢复默认对应表
    MAP.clear();BOX.clear();TARGET.clear();
    px=-1;py=-1;

    /* ---------- 地图部分：读到空行为止 ---------- */
    vector<string> RAW;
    string line;
    size_t W=0;
    while(getline(in,line))
    {
        if(!line.empty() && line[line.size()-1]=='\r') line.erase(line.size()-1);
        if(line.empty()) break;//空行后面是字符对应表
        RAW.push_back(line);
        if(line.size()>W) W=line.size();
    }

    /* ---------- 字符对应表：'原字符=显示字符' 空格隔开,只读一行 ---------- */
    while(getline(in,line))
    {
        if(!line.empty() && line[line.size()-1]=='\r') line.erase(line.size()-1);
        if(line.empty()) continue;
        string t;
        for(size_t i=0;i<=line.size();i++)
        {
            if(i==line.size() || line[i]==' ' || line[i]=='\t')
            {
                if(t.size()>=3 && t[1]=='=')//形如 "原字符=显示字符"
                    DISP[(unsigned char)t[0]]=t.substr(2);
                t.clear();
            }
            else t+=line[i];
        }
        break;
    }

    /* ---------- 补齐成矩形地图,顺便把箱子/玩家/目标点抠出来 ---------- */
    for(size_t y=0;y<RAW.size();y++)
    {
        string row=RAW[y];
        while(row.size()<W) row+='0';//0=空格,补齐
        for(size_t x=0;x<W;x++)
        {
            char c=row[x];
            pair<int,int> P=make_pair((int)x,(int)y);
                 if(c=='0') c=' ';
            else if(c=='$') {BOX.insert(P);        c=' ';}
            else if(c=='@') {px=(int)x;py=(int)y;  c=' ';}
            else if(c=='.') {TARGET.insert(P);     c=' ';}
            else if(c=='*') {BOX.insert(P);TARGET.insert(P);c=' ';}//箱子在目标点上
            else if(c=='+') {px=(int)x;py=(int)y;TARGET.insert(P);c=' ';}//玩家在目标点上
            row[x]=c;
        }
        MAP.push_back(row);
    }
    if(MAP.empty() || px<0 || TARGET.empty()) return 0;
    return 1;
}

/* ==================== 绘制地图 ==================== */

void draw_cell(const vector<string>& MAP,int x,int y,int px,int py)//画一格
{
    pair<int,int> P=make_pair(x,y);
    gotoxy((short)x,(short)y);
    if(x==px && y==py)
    {set_color(0,11);fputs(DISP['@'].c_str(),stdout);}
    else if(BOX.count(P))
    {
        if(TARGET.count(P)) {set_color(0,10);fputs(DISP['$'].c_str(),stdout);}//归位:绿
        else                {set_color(0,14);fputs(DISP['$'].c_str(),stdout);}//没归位:黄
    }
    else if(TARGET.count(P))    {set_color(0,12);fputs(DISP['.'].c_str(),stdout);}
    else if(MAP[y][x]=='#')     {set_color(0,15);fputs(DISP['#'].c_str(),stdout);}
    else if(MAP[y][x]=='7')     {set_color(0, 8);fputs(DISP['7'].c_str(),stdout);}
    else                        {set_color(0, 0);putchar(' ');}
}
void draw_map(const vector<string>& MAP,int px,int py)//绘制整个地图
{
    for(int y=0;y<(int)MAP.size();y++)
        for(int x=0;x<(int)MAP[y].size();x++)
            draw_cell(MAP,x,y,px,py);
}

/* ==================== 运行游戏 ==================== */

void run_game(vector<string>& MAP,int px,int py,const string& LEVEL_NAME)
{
    int GH=MAP.size();
    int GW=MAP[0].size();

    LARGE_INTEGER frequency,start,end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);//开始计时

    int STEP=0;
    int key,dx,dy,k,nx,ny,bx=0,by=0;
    bool PUSHED;
    for(;;)//主循环
    {
        key=_getch();
        if(key==27 || key=='q' || key=='Q')//随时退出
        {
            set_color(0,7);
            system("cls");
            printf("  箱子都笑话你。\n");
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
        if(nx<0 || ny<0 || nx>=GW || ny>=GH)     continue;
        if(MAP[ny][nx]=='#' || MAP[ny][nx]=='7') continue;//撞墙/撞边界

        PUSHED=0;
        pair<int,int> NP=make_pair(nx,ny);
        if(BOX.count(NP))//前面是箱子,得看看箱子前面能不能走
        {
            bx=nx+dx;by=ny+dy;
            if(bx<0 || by<0 || bx>=GW || by>=GH)     continue;
            if(MAP[by][bx]=='#' || MAP[by][bx]=='7') continue;//箱子撞墙
            if(BOX.count(make_pair(bx,by)))          continue;//箱子顶箱子,推不动
            BOX.erase(NP);
            BOX.insert(make_pair(bx,by));//推！
            PUSHED=1;
        }

        int opx=px,opy=py;
        px=nx;py=ny;STEP++;

        draw_cell(MAP,opx,opy,px,py);//旧位置还原(可能是目标点)
        if(PUSHED) draw_cell(MAP,bx,by,px,py);//箱子新位置
        draw_cell(MAP,px ,py ,px,py);//玩家新位置

        /* ---------- 全部箱子归位？ ---------- */
        int ON=0;
        for(set<pair<int,int> >::iterator t=TARGET.begin();t!=TARGET.end();t++)
            if(BOX.count(*t)) ON++;
        if(ON==(int)TARGET.size())
        {
            QueryPerformanceCounter(&end);//结束计时
            double ms=(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000;
            string BASE=base_name(LEVEL_NAME);//分数只记纯文件名,不带路径
            bool NEW_RECORD=save_score(BASE,STEP);
            end_screen(ms,STEP,SCORE_BEST[BASE],NEW_RECORD);
            return;
        }
    }
}

/* ==================== 主函数 ==================== */

int main()
{
    /* ---------- 初始设置 ---------- */
    SetConsoleOutputCP(65001);//UTF-8，保证中文不乱码
    SetConsoleCP(65001);
    hide_cursor();
    load_scores();

    start_screem();
    for(;;)
    {
        string LEVEL_NAME=choose_level();
        if(LEVEL_NAME.empty()) break;

        vector<string> MAP;
        int px=0,py=0;
        if(!load_level(LEVEL_NAME,MAP,px,py))
        {
            set_color(0,12);printf("  %s 读不出来，文件是不是坏了？\n",LEVEL_NAME.c_str());
            set_color(0,7 );system("pause");
            continue;
        }

        system("cls");
        draw_map(MAP,px,py);

        /* ---------- 底部简略引导 ---------- */
        gotoxy(0,(short)(MAP.size()+1));
        set_color(0,7);
        printf("  ↑↓←→ w/s/a/d 移动 | ESC/Q 退出 | 把箱子全部推到目标点上 ");

        run_game(MAP,px,py,LEVEL_NAME);

        /* ---------- 游戏结束提示 ---------- */
        set_color(0,11);printf("-------------------------------------------------\n");
        set_color(0,7 );printf("  按 ESC 或 Q 退出游戏，按任意键返回选关\n");
        fflush(stdout);
        int key=_getch();
        if(key==27 || key=='q' || key=='Q') break;
    }
    set_color(0,7);
    return 0;
}//我还没改完
/*依旧学习
1.ifstream
2.文件搜索
3.do{}while();
*/