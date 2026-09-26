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
#include<filesystem>
using namespace std;

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

string DISP[256];//---------------------------字符对应表:地图字符->显示字符
const string SCORE=R"(score.push_boxes)";//---成绩文件夹
const string DIR=R"(levels\)";//--------------关卡文件夹
const string NAME=R"(level_*)";//-------------关卡命名方式
const string EXTENTION=R"(.push_boxes)";//----关卡后缀
map<string,pair<int,int> > BESTS;//-----------每关历史最佳步数(名字)
vector<string> LEVELS;//----------------------关卡目录(名字)
string CHOSEN;//------------------------------选的关卡(名字)
vector<string> MAP;//-------------------------地图
int px,py;//---------------玩家位置
int TARGET,ON;//-----------目标/在目标上的箱子
int STEP,MS;//-------------步/时

string base_name(string s)//-------------------------去掉路径,只留文件名
{
    size_t p=s.find_last_of("\\/");
        s= p==string::npos?s:s.substr(p+1);
           p=s.find_last_of(".");
    return p==string::npos?s:s.substr(0,p);
}
int level_num(const string& s)//---------------------从文件名里抠出关卡号
{
    int n=0;
    for(size_t i=0;i<s.size();i++)
        if(s[i]>='0' && s[i]<='9') n=n*10+s[i]-'0';
    return n;
}
void find_level()
{
    WIN32_FIND_DATAA a;
    HANDLE HF=FindFirstFileA((DIR+NAME+EXTENTION).c_str(),&a);//超级拼装
    if(HF!=INVALID_HANDLE_VALUE)
    {
        do{
            LEVELS.push_back(base_name(a.cFileName));}//存
        while(FindNextFileA(HF,&a));
        FindClose(HF);
    }
    for(size_t i=0;i<LEVELS.size();i++) for(size_t j=i+1;j<LEVELS.size();j++)
            if(level_num(LEVELS[i])>level_num(LEVELS[j])) swap(LEVELS[i],LEVELS[j]);//排序
}

/* ==================== 游戏存取 ==================== */

void load_scores()//----------------------------------读历史最佳
{
    ifstream in(SCORE);
    string name;
    int step,ms;
    while(in>>name>>step>>ms)
        BESTS[name].first  =step,
        BESTS[name].second =ms  ;
}
bool save_score()//--破纪录才更新,返回是否新纪录
{
    map<string,pair<int,int> >::iterator it=BESTS.find(CHOSEN);
    if(it!=BESTS.end())
    {
        if(it->second.first<STEP)                           return 0;//步数更差
        if(it->second.first==STEP && it->second.second<=MS) return 0;//秒数不小
    }
    BESTS[CHOSEN].first  =STEP;
    BESTS[CHOSEN].second =MS  ;
    ofstream out(SCORE+".tmp");//临时文件,防止写到一半停了
    for(it=BESTS.begin();it!=BESTS.end();it++)
        out <<it->first        <<" "
            <<it->second.first <<" "
            <<it->second.second<<"\n";
    rename((SCORE+".tmp").c_str(),SCORE.c_str());//合并
    out.close();//防止阻碍删除
    remove((SCORE+".tmp").c_str());
    return 1;
}
bool load_level()//----------------------------------读关卡文件
{
    ifstream in(DIR+CHOSEN+EXTENTION);//选的关卡路径
    if(!in.is_open()) return 0;

    /* ---------- 读地图 ---------- */
    string line;
    while(getline(in,line))
    {
        if(!line.empty() && line[line.size()-1]=='\r') line.erase(line.size()-1);//去\r
        if( line.empty()) break;//空行
        MAP.push_back(line);
    }

    /* ---------- 字符对应表：'原字符=显示字符' 空格隔开,只读一行 ---------- */
    while(getline(in,line))
    {
        if(!line.empty() && line[line.size()-1]=='\r') line.erase(line.size()-1);//去\r
        if( line.empty()) continue;
        string t="";
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

    /* ---------- 找箱子/玩家/目标 ---------- */
    int BOX=0,PLAYER=0;//另外两个已经初始化了
    for(size_t y=0;y<MAP.size();y++) for(size_t x=0;x<MAP[y].size();x++)
    {
        switch(MAP[y][x])
        {
            case '$':BOX++;                      break;
            case '@':px=x;py=y;PLAYER++;         break;
            case '.':                   TARGET++;break;
            case '*':BOX++;ON++;        TARGET++;break;//箱子在目标点上
            case '+':px=x;py=y;PLAYER++;TARGET++;break;//玩家在目标点上
            case '#':case '7':case ' ':break;
            default: return 0;//非法字符
        }
    }
    if(MAP.empty() || px<0 || TARGET>BOX || !TARGET || PLAYER!=1) return 0;
    return 1;
}

/* ==================== 界面 ==================== */

void start_screem()//--------------------------------开始界面
{
    system("cls");
    set_color(0,10);printf("=================================================\n");
                    printf("                     推 箱 子\n");
                    printf("=================================================\n");
    set_color(0,12);printf("[操作引导]\n");
    set_color(0, 7);printf("  ↑↓←→  w/s/a/d ：移动\n");
                    printf("  ESC / Q       ：退出游戏\n");
                    printf("  把箱子全部推到目标点上就过关\n");
    set_color(0,12);printf("[温馨提醒]\n");
    set_color(0, 7);printf("  你的控制台尽量大点，不然……\n");
    set_color(0,11);printf("-------------------------------------------------\n");
    set_color(0, 7);printf("  按任意键继续……\n\n");
    fflush(stdout);
    _getch();
}
string choose_level()//------------------------------选关界面
{
    set_color(0,7);//先恢复默认颜色再清屏，防止背景残留游戏配色
    system("cls");
    set_color(0,10);printf("================================================\n");
                    printf("                    选 关 卡\n");
                    printf("================================================\n");
    if(LEVELS.empty())
    {
        set_color(0,12);printf("  我关卡文件呢?\n");
        set_color(0, 7);printf("  %s里怎么啥也没有?\n",CHOSEN.c_str());
        system("pause");
        return "";
    }
    for(size_t i=0;i<LEVELS.size();i++)
    {
        set_color(0,14);printf(" %2d. ",(int)(i+1));
        set_color(0, 7);printf("%-9s",LEVELS[i].c_str());
        map<string,pair<int,int> >::iterator it=BESTS.find(LEVELS[i]);
        if(it!=BESTS.end()){
            set_color(0,10);printf("  最佳: %3d 步  用时 %8.3lf 秒",it->second.first,it->second.second/1000.0);}
        else{
            set_color(0, 8);printf("                    还没人征服过");}
        putchar('\n');
    }
    set_color(0,11);printf("-------------------------------------------------\n");
    set_color(0, 7);printf("  输入编号选关，乱输我直接给你退了: ");
    fflush(stdout);
    string s;
    getline(cin,s);
    int n=atoi(s.c_str());
    if(n>=1 && n<=(int)LEVELS.size()) return LEVELS[n-1];
    return "";
}
void end_screen()//----------------------------------通关画面
{
    set_color(0,7);//先恢复默认颜色再清屏，防止背景残留游戏配色
    system("cls");
    int    m=MS/60000;
    double s=(MS-m*60000)/1000.0;
    bool   NEW=save_score();

    set_color(0,10);printf("\n================================================\n");
    set_color(0,14);printf("            * * *  ");
    set_color(0,12);printf("你 过 关 !");
    set_color(0,14);printf("  * * *\n");
    set_color(0,10);printf("================================================\n\n");
    set_color(0,14);printf("          *    *    *    *    *    *\n");
                    printf("        *      Y O U   W I N ! !     *\n");
                    printf("          *    *    *    *    *    *\n\n");
    set_color(0, 7);printf("  通关总耗时: ");
    if(m>0){
        set_color(0,11);printf("%2d 分 %.3lf 秒\n",m,s);}
    else{
        set_color(0,11);printf("%.3lf 秒\n",s);}

        set_color(0, 7);printf("  消耗步数: ");
        set_color(0,11);printf("%3d 步\n",STEP);
        set_color(0, 7);printf("  历史最佳: ");
    if(NEW){
        set_color(0,10);printf("%3d 步 %8.3lf 秒  新纪录！裱起来！\n",STEP,MS/1000.0);}
    else{
        set_color(0,11);printf("%3d 步 %8.3lf 秒\n",BESTS[CHOSEN].first,BESTS[CHOSEN].second/1000.0);}
    set_color(0, 7);printf("  按 ENTER 键继续……");
    fflush(stdout);
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

/* ==================== 绘制地图 ==================== */

void draw_cell(int x,int y)//------------------------画一格
{
    gotoxy(x,y);
    switch(MAP[y][x])
    {
        case '$':set_color(0,14);fputs(DISP['$'].c_str(),stdout);break;//没归位:黄
        case '@':set_color(0,11);fputs(DISP['@'].c_str(),stdout);break;
        case '.':set_color(0,12);fputs(DISP['.'].c_str(),stdout);break;
        case '*':set_color(0,10);fputs(DISP['*'].c_str(),stdout);break;//归位:绿
        case '+':set_color(0,12);fputs(DISP['+'].c_str(),stdout);break;//玩家在目标:红
        case '#':set_color(0,15);fputs(DISP['#'].c_str(),stdout);break;
        case '7':set_color(0, 8);fputs(DISP['7'].c_str(),stdout);break;//边界
        case ' ':set_color(0, 0);putchar(' ');                   break;
    }
}
void draw_map()//------------------------------------绘制整个地图
{
    system("cls");
    for(size_t y=0;y<MAP.size();y++) for(size_t x=0;x<MAP[y].size();x++)
        draw_cell(x,y);
}

/* ==================== 运行游戏 ==================== */

void map_change(int x,int y,int nx,int ny)//---------修改地图
{
    switch(MAP[y][x])
    {
        case '$': switch(MAP[ny][nx]){
                case '.':MAP[ny][nx]='*';ON++;break;
                case ' ':MAP[ny][nx]='$';break;
            }MAP[y][x]=' ';break;
        case '@': switch(MAP[ny][nx]){
                case '.':MAP[ny][nx]='+';break;
                case ' ':MAP[ny][nx]='@';break;
            }MAP[y][x]=' ';break;
        case '*': switch(MAP[ny][nx]){
                case '.':MAP[ny][nx]='*';ON++;break;
                case ' ':MAP[ny][nx]='$';break;
            }MAP[y][x]='.';ON--;break;
        case '+': switch(MAP[ny][nx]){
                case '.':MAP[ny][nx]='+';break;
                case ' ':MAP[ny][nx]='@';break;
            }MAP[y][x]='.';break;
    }
}
void run_game()//------------------------------------进行游戏
{
    LARGE_INTEGER frequency,start,end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);//开始计时

    int key,dx,dy,nx,ny,bx,by;
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
        switch(key)
        {
            case 0:case 224:// 功能键 / 方向键
                key=_getch();
                switch(key){
                    case 72:  dy=-1;break;// ↑
                    case 80:  dy= 1;break;// ↓
                    case 75:  dx=-1;break;// ←
                    case 77:  dx= 1;break;// →
                }break;
            case 'w':case 'W':dy=-1;break;
            case 's':case 'S':dy= 1;break;
            case 'a':case 'A':dx=-1;break;
            case 'd':case 'D':dx= 1;break;
        }
        if(dx==0 && dy==0) continue;

        nx=px+dx;ny=py+dy;
        if(ny<0 || ny>=(int)MAP.size() || nx<0 || nx>=(int)MAP[ny].size()) continue;//防止越界
        if(MAP[ny][nx]=='#' || MAP[ny][nx]=='7') continue;//撞墙/边界

        PUSHED=0;
        if(MAP[ny][nx]=='$' || MAP[ny][nx]=='*')//前面是箱子
        {
            bx=nx+dx;by=ny+dy;
            if(by<0 || by>=(int)MAP.size() || bx<0 || bx>=(int)MAP[by].size()) continue;//防止越界
            if(MAP[by][bx]=='#' || MAP[by][bx]=='7') continue;//箱子撞墙/边界
            if(MAP[by][bx]=='$' || MAP[by][bx]=='*') continue;//箱子顶箱子
            PUSHED=1;
        }
        if(PUSHED)
        {
            map_change(nx,ny,bx,by);//推！
            draw_cell(bx,by);
        }
        map_change(px,py,nx,ny);//走！
        draw_cell(nx,ny);draw_cell(px,py);

        px=nx;py=ny;STEP++;

        /* ---------- 判断箱子归位 ---------- */
        if(ON==TARGET)
        {
            QueryPerformanceCounter(&end);//结束计时
            MS=(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000;
            end_screen();
            return;
        }
    }
}

/* ==================== 初始化 ==================== */

inline void init()
{
    px=py=-1;
    STEP=ON=TARGET=0;
    for(int i=0;i<256;i++) DISP[i]=string(1,i);//映射
    //墙#▓  边界7▒  箱子$□  *■  目标.○  玩家@☻  +☺//
     BESTS.clear();
    LEVELS.clear();
       MAP.clear();
    load_scores();
     find_level();
}

/* ==================== 主函数 ==================== */

int main()
{
    /* ---------- 初始设置 ---------- */
    SetConsoleOutputCP(65001);//UTF-8，保证中文不乱码
    SetConsoleCP(65001);
    hide_cursor();

    start_screem();
    int key;
    for(;;)
    {
        init();

        CHOSEN=choose_level();
        if(CHOSEN.empty()) break;

        if(!load_level())
        {
            set_color(0,12);printf("  %s 读不出来，文件是不是坏了?\n",DIR.c_str());
            set_color(0, 7);system("pause");
            continue;
        }

        draw_map();

        /* ---------- 底部简略引导 ---------- */
        gotoxy(0,MAP.size()+1);
        set_color(0,7);
        printf("  ↑↓←→ w/s/a/d 移动 | ESC/Q 退出 | 把箱子全部推到目标点上");

        run_game();

        /* ---------- 游戏结束提示 ---------- */
        set_color(0,11);printf("-------------------------------------------------\n");
        set_color(0, 7);printf("  按 ESC 或 Q 退出游戏，按任意键返回选关\n");
        fflush(stdout);
        key=_getch();
        if(key==27 || key=='q' || key=='Q') break;
    }
    set_color(0,7);
    return 0;
}
/*依旧学习
1.ifstream   ofstream
2.文件搜索
3.do{}while();
4.set存变量
5.比较size()最好用size_t
*/