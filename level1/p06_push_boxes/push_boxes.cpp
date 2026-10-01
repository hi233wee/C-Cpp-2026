#include<iostream>
#include<vector>
#include<string>
#include<utility>
#include<cstdlib>
#include<random>
#include<cstdio>
#include<conio.h>
#include<windows.h>
#include<fstream>
#include<map>
#include<filesystem>
using namespace std;

random_device RD;
mt19937 RNG(RD());
uniform_int_distribution<int> RAND_COLOR(1,15);//1-15随机颜色,避开黑色
inline int Rand_color();

HANDLE handle_output=GetStdHandle(STD_OUTPUT_HANDLE);
const int min_W=51;//最小宽度(底部提示49列)
const int min_H=14;//最小高度(结束界面13)

void set_console_font();
void lock_window();
void set_console_size(SHORT W, SHORT H);
void hide_cursor();
void gotoxy(short x, short y);
void get_console_size(int& w, int& h);
void set_color(int BG, int FG);

const char* COLOR_NAMES[16]=//---颜色模版
{"黑色","深蓝","深绿","青色","深红","紫色","暗黄","浅灰","深灰","亮蓝","亮绿","亮青","亮红","亮紫","黄色","白色"};
// 0      1      2      3     4      5      6      7     8      9     10     11     12     13    14     15  //

const string DIR=R"(levels\)";//--------------关卡文件夹
const string NAME=R"(level_*)";//-------------关卡命名方式
const string EXTENTION=R"(.push_boxes)";//----关卡后缀
vector<string> LEVELS;//----------------------关卡目录(名字)
string CHOSEN;//------------------------------选的关卡(名字)
string base_name(string s);
int  level_num(const string& s);
void find_level();

const string SCORE=R"(score.push_boxes)";//---成绩文件夹
map<string,pair<int,int> > BESTS;//-----------每关历史最佳步数和时间(名字)
void load_scores();
bool save_score();
bool load_level();

void start_screen();
string choose_level();
void end_screen();

string DISP[256];//---------------------------字符对应表:地图字符->显示字符
vector<string> MAP;//-------------------------地图
int px,py,OX,OY;//---------玩家位置，地图起始位置
int TARGET,ON;//-----------目标/在目标上的箱子
int STEP,MS;//-------------步/时
int W,H,MW;//--------------控制台大小，地图大小
void draw_cell(int x, int y);
void draw_map();

void map_change(int x, int y, int nx, int ny);
void run_game();

inline void init();

bool is_wide(wchar_t c);
int  disp_width(wstring s);
void put_line(SHORT x, SHORT y, WORD attr, wstring s);

int main() {
    //初始设置//
    SetConsoleOutputCP(65001);//UTF-8，保证中文不乱码
    SetConsoleCP(65001);
    set_console_font();
    lock_window();
    set_console_size(0,0);
    hide_cursor();

    start_screen();
    int key;
    for(;;) {
        init();

        CHOSEN=choose_level();
        if(CHOSEN.empty()) break;

        if(!load_level()) {
            set_color(0,12);printf("  %s 读不出来，文件是不是坏了?\n",DIR.c_str());
            set_color(0, 7);system("pause");
            continue;
        }

        set_console_size(MW+2,MAP.size()+4);//留点空
        get_console_size(W,H);
        OX=(W-MW)/2; OY=(H-MAP.size())/2-1;
        draw_map();

        //底部简略引导//
        gotoxy((W-49)/2,H-2);
        set_color(0,7);
        printf("↑↓←→ w/s/a/d 移动 | ESC/Q 退出 | 让箱子填满目标点");

        run_game();

        //游戏结束提示//
        set_color(0,11);printf(" -------------------------------------------------\n");
        set_color(0, 7);printf("   按 ESC 或 Q 退出游戏，按任意键返回选关\n");
        key=_getch();
        if(key==27 || key=='q' || key=='Q') break;
    }
    set_color(0,7);
    return 0;
}//推箱子

//小插件//

bool is_wide(wchar_t c) {//-----------------------------判断字符是否占两格
    WORD t=0;
    GetStringTypeW(CT_CTYPE3, &c, 1, &t);
    return (t & (C3_FULLWIDTH|C3_IDEOGRAPH)) != 0;
}

int disp_width(wstring s) {//---------------------------测宽字符串长度
    int w=0;
    for (wchar_t c : s) {
        //通过获取字符属性累计//
        WORD t=0;
        GetStringTypeW(CT_CTYPE3, &c, 1, &t);
        w += (t & (C3_FULLWIDTH|C3_IDEOGRAPH)) ? 2 : 1;
    }
    return w;
}

void put_line(SHORT x, SHORT y, WORD A, wstring s) {//--按位置输出一行带颜色字符串
    int w=disp_width(s);
    vector<CHAR_INFO> Buf(w);
    int cx=0;//屏幕列位置
    for (size_t i=0; i<s.size(); i++) {
        if (is_wide(s[i]) && cx+1<w) {//宽字符占两格, 两格都要标 LVB
            Buf[cx  ].Char.UnicodeChar = s[i];
            Buf[cx  ].Attributes       = A|COMMON_LVB_LEADING_BYTE ;
            Buf[cx+1].Char.UnicodeChar = L' ';
            Buf[cx+1].Attributes       = A|COMMON_LVB_TRAILING_BYTE;
            cx += 2;
        } else {
            Buf[cx].Char.UnicodeChar = s[i];
            Buf[cx].Attributes       = A;
            cx += 1;
        }
    }
    COORD      size    = { (SHORT)w, 1 };
    COORD      Buf_Org = { 0, 0 };
    SMALL_RECT region  = { x, y, (SHORT)(x+w-1), y};
    WriteConsoleOutputW(handle_output, Buf.data(), size, Buf_Org, &region);
}

//初始化//

inline void init() {//----------------------------------初始化一局的全部数据
    px=py=-1;
    STEP=ON=TARGET=MW=0;
    for(int i=0;i<256;i++) DISP[i]=string(1,i);//映射
    //墙#▓  边界7▒  箱子$□  *■  目标.○  玩家@☻  +☺//
    BESTS.clear();
    LEVELS.clear();
       MAP.clear();
    load_scores();
    find_level();
}

//运行//

void map_change(int x, int y, int nx, int ny) {//-------修改地图
    switch(MAP[y][x]) {
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

void run_game() {//-------------------------------------进行游戏
    LARGE_INTEGER frequency,start,end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);//开始计时

    int key,dx,dy,nx,ny,bx,by;
    bool PUSHED;
    for(;;) {//主循环
        key=_getch();
        if(key==27 || key=='q' || key=='Q') {//随时退出
            set_color(0,7);
            system("cls");
            printf("   箱子都笑话你。\n");
            return;
        }

        dx=0,dy=0;
        switch(key) {
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
        if(MAP[ny][nx]=='$' || MAP[ny][nx]=='*') {//前面是箱子
            bx=nx+dx;by=ny+dy;
            if(by<0 || by>=(int)MAP.size() || bx<0 || bx>=(int)MAP[by].size()) continue;//防止越界
            if(MAP[by][bx]=='#' || MAP[by][bx]=='7') continue;//箱子撞墙/边界
            if(MAP[by][bx]=='$' || MAP[by][bx]=='*') continue;//箱子顶箱子
            PUSHED=1;
        }
        if(PUSHED) {
            map_change(nx,ny,bx,by);//推！
            draw_cell(bx,by);
        }
        map_change(px,py,nx,ny);//走！
        draw_cell(nx,ny);draw_cell(px,py);

        px=nx;py=ny;STEP++;

        //判断箱子归位//
        if(ON==TARGET) {
            QueryPerformanceCounter(&end);//结束计时
            MS=(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000;
            end_screen();
            return;
        }
    }
}

//绘制//

void draw_cell(int x, int y) {//------------------------画一格
    gotoxy(x+OX,y+OY);
    switch(MAP[y][x]) {
        case '$':set_color(0, 6);fputs(DISP['$'].c_str(),stdout);break;//没归位:暗黄
        case '@':set_color(0,11);fputs(DISP['@'].c_str(),stdout);break;
        case '.':set_color(0,12);fputs(DISP['.'].c_str(),stdout);break;
        case '*':set_color(0,10);fputs(DISP['*'].c_str(),stdout);break;//归位:绿
        case '+':set_color(0, 3);fputs(DISP['+'].c_str(),stdout);break;//玩家在目标:暗青
        case '#':set_color(0,15);fputs(DISP['#'].c_str(),stdout);break;
        case '7':set_color(0, 8);fputs(DISP['7'].c_str(),stdout);break;//边界
        case ' ':set_color(0, 0);putchar(' ');                   break;
    }
}

void draw_map() {//-------------------------------------绘制整个地图
    system("cls");
    for(size_t y=0;y<MAP.size();y++) for(size_t x=0;x<MAP[y].size();x++)
        draw_cell(x,y);
}

//界面//

void start_screen() {//---------------------------------开始界面
    system("cls");
    put_line( 2, 1, 7,L"╔════════════════════════════════════════════════╗");
    put_line( 2, 2, 7,L"║                                                ║");
    put_line( 2, 3, 7,L"║                                                ║");
    put_line( 2, 4, 7,L"║                                                ║");
    put_line( 2, 5, 7,L"║                                                ║");
    put_line( 2, 6, 7,L"╟────────────────────────────────────────────────╢");
    put_line( 2, 7, 7,L"║  [操作引导]                                    ║");
    put_line( 2, 8, 7,L"║    w/s/a/d 或方向键 ：移动                     ║");
    put_line( 2, 9, 7,L"║    ESC / Q           ：退出游戏                ║");
    put_line( 2,10, 7,L"║    把箱子全部推到目标点上就过关                ║");
    put_line( 2,11, 7,L"╟────────────────────────────────────────────────╢");
    put_line( 2,12, 7,L"║                按任意键继续...                 ║");
    put_line( 2,13, 7,L"╚════════════════════════════════════════════════╝");
    put_line(21, 3,10,L"┌──────────┐");
    put_line(21, 4,10,L"│ 推 箱 子 │");
    put_line(21, 5,10,L"└──────────┘");
    put_line( 5, 7,12,L"[操作引导]");
    put_line(19,12,11,L"按任意键继续...");
    _getch();
}

string choose_level() {//-------------------------------选关界面
    set_console_size(0,LEVELS.size()+6);
    system("cls");
    put_line( 2, 1, 7,L"╔════════════════════════════════════════════════╗");
    put_line( 2, 2, 7,L"║                                                ║");
    put_line( 2, 3, 7,L"╟────────────────────────────────────────────────╢");
    for(size_t i=0;i<LEVELS.size();i++)
        put_line( 2,(SHORT)(4+i), 7,L"║                                                ║");
    put_line( 2,(SHORT)(4+LEVELS.size()), 7,L"╟────────────────────────────────────────────────╢");
    put_line( 2,(SHORT)(5+LEVELS.size()), 7,L"║  输入编号选关 (乱输直接退出):                  ║");
    put_line( 2,(SHORT)(6+LEVELS.size()), 7,L"╚════════════════════════════════════════════════╝");
    put_line(22, 2,10,L"选 关 卡");
    //没有关卡//
    if(LEVELS.empty()) {
        set_color(0,12);printf("   我关卡文件呢?\n");
        set_color(0, 7);printf("   %s里怎么啥也没有?\n",CHOSEN.c_str());
        set_color(0, 7);printf("按任意键继续...\n\n");
        _getch();
        return "";
    }
    //逐关列出//
    for(size_t i=0;i<LEVELS.size();i++) {
        gotoxy(4,(SHORT)(4+i));
        //编号与名字//
        set_color(0, 6);printf("%2d. ",(int)(i+1));
        set_color(0, 7);printf("%-9s",LEVELS[i].c_str());
        //历史最佳//
        map<string,pair<int,int> >::iterator it=BESTS.find(LEVELS[i]);
        if(it!=BESTS.end()){
            set_color(0, 3);printf("  最佳: ");
            set_color(0,11);printf("%3d",it->second.first);
            set_color(0, 3);printf(" 步  用时 ");
            set_color(0,11);printf("%8.3lf",it->second.second/1000.0);
            set_color(0, 3);printf(" 秒");}
        else{
            set_color(0, 8);printf("                    还没人征服过");}
    }
    //输入//
    gotoxy(36,(SHORT)(5+LEVELS.size()));
    set_color(0,7);
    string s;
    getline(cin,s);
    int n=atoi(s.c_str());
    if(n>=1 && n<=(int)LEVELS.size()) return LEVELS[n-1];
    return "";
}

void end_screen() {//-----------------------------------通关画面
    int    m=MS/60000;
    double s=(MS-m*60000)/1000.0;
    bool   NEW=save_score();
    system("cls");

    put_line( 2, 1, 7,L"╔════════════════════════════════════════════════╗");
    put_line( 2, 2,14,L"║      * * *        你 过 关 !        * * *      ║");
    put_line( 2, 3,10,L"╟────────────────────────────────────────────────╢");
    put_line( 2, 4,14,L"║           *    *    *    *    *    *           ║");
    put_line( 2, 5,14,L"║         *      Y O U   W I N ! !     *         ║");
    put_line( 2, 6,14,L"║           *    *    *    *    *    *           ║");
    put_line( 2, 7,10,L"╟────────────────────────────────────────────────╢");
    put_line( 2, 8, 7,L"║   通关总耗时:                                  ║");
    put_line( 2, 9, 7,L"║   消耗步数:                                    ║");
    put_line( 2,10, 7,L"║   历史最佳:                                    ║");
    put_line( 2,11,10,L"╟────────────────────────────────────────────────╢");
    put_line( 2,12, 7,L"║               按 ENTER 键继续...               ║");
    put_line( 2,13, 7,L"╚════════════════════════════════════════════════╝");
    //耗时//
    gotoxy(17,8);
    set_color(0,11);
    if(m>0) printf("%2d 分 %.3lf 秒\n",m,s);
    else    printf("%.3lf 秒\n",s);
    //步数//
    gotoxy(17,9);
    set_color(0,11);
    printf("%3d 步\n",STEP);
    //最佳//
    gotoxy(17,10);
    if(NEW){
        set_color(0,10);printf("%3d 步 %8.3lf 秒  新纪录！裱起来！\n",STEP,MS/1000.0);}
    else{
        set_color(0,11);printf("%3d 步 %8.3lf 秒\n",BESTS[CHOSEN].first,BESTS[CHOSEN].second/1000.0);}
    //闪烁+退出//
    for(;;) {
        put_line(19,5,Rand_color(),L"Y O U   W I N ! !");
        Sleep(150);
        if(_kbhit()) if(_getch()==13) break;
    }
    gotoxy(0,13);//光标返回
}

//存取//

void load_scores() {//----------------------------------读历史最佳
    ifstream in(SCORE);
    if(!in.is_open()){
        set_color(0,12);printf("   出了点小问题, 成绩读取失败了\n");
        set_color(0,14);printf("   但是并不影响你玩, 对吗?\n");
        set_color(0, 7);printf("按任意键继续...\n\n");
        _getch();
        return;
    }
    string name;
    int step,ms;
    while(in>>name>>step>>ms)
        BESTS[name].first  =step,
        BESTS[name].second =ms  ;
}

bool save_score() {//-----------------------------------保存,返回是否新纪录
    map<string,pair<int,int> >::iterator it=BESTS.find(CHOSEN);
    if(it!=BESTS.end()) {
        if(it->second.first<STEP)                           return 0;//步数更差
        if(it->second.first==STEP && it->second.second<=MS) return 0;//秒数不小
    }
    BESTS[CHOSEN].first  =STEP;
    BESTS[CHOSEN].second =MS  ;
    ofstream out(SCORE+".tmp");//临时文件,防止写到一半停了
    for(it=BESTS.begin();it!=BESTS.end();++it)
        out <<it->first        <<" "
            <<it->second.first <<" "
            <<it->second.second<<"\n";
    out.close();//防止阻碍合并
    error_code ec;
    filesystem::rename(SCORE+".tmp",SCORE,ec);//合并
    if(ec){//有错误信息
        set_color(0,12);printf("   emmm, 坏了, 保存失败了\n");
        set_color(0,14);printf("   哎呀不管了, 先祝贺你再说\n");
        set_color(0, 7);printf("按任意键继续...\n\n");
        _getch();
    }
    return 1;
}

bool load_level() {//-----------------------------------读关卡文件
    ifstream in(DIR+CHOSEN+EXTENTION);//选的关卡路径
    if(!in.is_open()) return 0;

    //读地图//
    string line;
    while(getline(in,line)) {
        if(!line.empty() && line[line.size()-1]=='\r') line.erase(line.size()-1);//去\r
        if( line.empty()) break;//空行
        MAP.push_back(line);
        MW= MW<line.size()?line.size():MW;
    }

    //字符对应表: '原字符=显示字符' 空格隔开,只读一行//
    while(getline(in,line)) {
        if(!line.empty() && line[line.size()-1]=='\r') line.erase(line.size()-1);//去\r
        if( line.empty()) continue;
        string t="";
        for(size_t i=0;i<=line.size();i++) {
            if(i==line.size() || line[i]==' ' || line[i]=='\t') {
                if(t.size()>=3 && t[1]=='=')//形如 "原字符=显示字符"
                    DISP[(unsigned char)t[0]]=t.substr(2);
                t.clear();
            }
            else t+=line[i];
        }
        break;
    }

    //找箱子/玩家/目标//
    int BOX=0,PLAYER=0;//另外两个已经初始化了
    for(size_t y=0;y<MAP.size();y++) for(size_t x=0;x<MAP[y].size();x++) {
        switch(MAP[y][x]) {
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

//关卡//

string base_name(string s) {//--------------------------去掉路径,只留文件名
    size_t p=s.find_last_of("\\/");
        s= p==string::npos?s:s.substr(p+1);
           p=s.find_last_of(".");
    return p==string::npos?s:s.substr(0,p);
}

int level_num(const string& s) {//----------------------从文件名里抠出关卡号
    int n=0;
    for(size_t i=0;i<s.size();i++)
        if(s[i]>='0' && s[i]<='9') n=n*10+s[i]-'0';
    return n;
}

void find_level() {//-----------------------------------扫描关卡文件夹
    WIN32_FIND_DATAA a;
    HANDLE HF=FindFirstFileA((DIR+NAME+EXTENTION).c_str(),&a);//超级拼装
    if(HF!=INVALID_HANDLE_VALUE) {
        do{
            LEVELS.push_back(base_name(a.cFileName));}//存
        while(FindNextFileA(HF,&a));
        FindClose(HF);
    }
    for(size_t i=0;i<LEVELS.size();i++) for(size_t j=i+1;j<LEVELS.size();j++)
            if(level_num(LEVELS[i])>level_num(LEVELS[j])) swap(LEVELS[i],LEVELS[j]);//排序
}

//控制台//

void set_console_font() {//-----------------------------改字体
    CONSOLE_FONT_INFOEX C_font = {};
    C_font.cbSize       = sizeof(C_font);
    C_font.dwFontSize.Y = 16;
    wcscpy_s(C_font.FaceName, L"Lucida Console");
    SetCurrentConsoleFontEx(handle_output, FALSE, &C_font);
}

void lock_window() {//----------------------------------不让动窗口大小
    HWND     C_W   = GetConsoleWindow();
    LONG_PTR W_LP  = GetWindowLongPtrA(C_W, GWL_STYLE);
    SetWindowLongPtrA(C_W, GWL_STYLE, W_LP & ~WS_THICKFRAME);
}

void set_console_size(SHORT W, SHORT H) {//-------------设窗口和缓冲区大小
    //规范W,H//
    if(W<min_W) W=min_W;
    if(H<min_H) H=min_H;
    COORD C_W_max     = GetLargestConsoleWindowSize(handle_output);
    if(W> C_W_max.X) W=C_W_max.X;
    if(H> C_W_max.Y) H=C_W_max.Y;
    //放大缓冲区//
    CONSOLE_SCREEN_BUFFER_INFO Buf_info;
    GetConsoleScreenBufferInfo(handle_output, &Buf_info);
    COORD Buf_size    = { Buf_info.dwSize.X <W ? W : Buf_info.dwSize.X,
                          Buf_info.dwSize.Y <H ? H : Buf_info.dwSize.Y };
    SetConsoleScreenBufferSize(handle_output, Buf_size);
    //设窗口大小//
    SMALL_RECT W_size = { 0, 0, SHORT(W-1), SHORT(H-1) };
    SetConsoleWindowInfo(handle_output, TRUE, &W_size);
    SetConsoleScreenBufferSize(handle_output, {W,H}  );//缩缓冲区到没有滚动条
}

void hide_cursor() {//----------------------------------隐藏光标
    CONSOLE_CURSOR_INFO Cur_info;
    Cur_info.bVisible =FALSE;
    Cur_info.dwSize   =1;
    SetConsoleCursorInfo(handle_output, &Cur_info);
}

void gotoxy(short x, short y) {//-----------------------移动光标
    COORD Cur_pos={x,y};
    SetConsoleCursorPosition(handle_output, Cur_pos);
}

void get_console_size(int& w, int& h) {//---------------获取控制台宽高
    CONSOLE_SCREEN_BUFFER_INFO Buf_info;
    GetConsoleScreenBufferInfo(handle_output, &Buf_info);
    w = Buf_info.srWindow.Right  - Buf_info.srWindow.Left +1;
    h = Buf_info.srWindow.Bottom - Buf_info.srWindow.Top  +1;
}

void set_color(int BG, int FG) {//----------------------设置颜色, BG背景, FG前景
    SetConsoleTextAttribute(handle_output, (WORD)((BG<<4)|FG));
}

//随机函数//

inline int Rand_color() {//-----------------------------随机颜色
    return RAND_COLOR(RNG);
}

/*依旧学习
1.ifstream   ofstream
2.文件搜索
3.do{}while();
4.set存变量
5.比较size()最好用size_t
6.for里用++i更好
7.filesystem文件操作
*/
