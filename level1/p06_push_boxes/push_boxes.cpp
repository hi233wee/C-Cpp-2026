#include <random>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <map>
#include <filesystem>
using namespace std;

random_device RD;
mt19937 RNG(RD());
uniform_int_distribution<int> RAND_COLOR(1, 15); //1-15随机颜色,避开黑色
inline int Rand_color ();

HANDLE H_out=GetStdHandle(STD_OUTPUT_HANDLE);
SHORT C_col, C_row; //窗口大小
const int min_W=54; //最小宽度(选关50+左右各留2)
const int min_H=17; //最小高度(选关15+上下各留1)
void set_console_font ();
void lock_window ();
void set_console_size (SHORT W, SHORT H);
void hide_cursor ();
void gotoxy (SHORT x, SHORT y);
void get_console_size (auto& col, auto& row);
void set_color (int BG, int FG);

const char* COLOR_NAMES[16]= //---颜色模版
{"黑色","深蓝","深绿","青色","深红","紫色","暗黄","浅灰","深灰","亮蓝","亮绿","亮青","亮红","亮紫","黄色","白色"};
// 0      1      2      3     4      5      6      7     8      9     10     11     12     13    14     15  //

bool is_wide (wchar_t c);
int  disp_width (wstring s);
void put_line (SHORT x, SHORT y, WORD A, wstring s);

vector<string> LEVELS; //----------------------关卡目录(名字)
string base_name (string s);
int  level_num (const string& s);
void find_level ();

const string SCORE=R"(score.push_boxes)"; //---成绩文件夹
const string SKIN=R"(skins.push_boxes)"; //----皮肤文件
const string DIR=R"(levels\)"; //--------------关卡文件夹
const string NAME=R"(level_*)"; //-------------关卡命名方式
const string EXTENTION=R"(.push_boxes)"; //----关卡后缀
map<string, pair<int, int> > BESTS; //---------每关历史最佳步数和时间(名字)
string DISP[256]; //---------------------------字符映射
void load_scores ();
bool save_score ();
void load_skins ();
bool load_level ();

string CHOSEN; //----------------选的关卡(名字)
vector<string> MAP; //-----------地图
int TARGET, ON; //---------------目标/被填满的目标
int px, py; //-------------------玩家初始位置
size_t map_W; //-----------------地图宽
int OX, OY; //-------------------偏移量
int STEP, MS; //-----------------步数/秒数

void main_screen ();
void choose_screen ();
void skin_screen ();
bool end_screen ();

void draw_cell (int x, int y);
void draw_map ();

void start ();
void map_change (int x, int y, int nx, int ny);
bool run_game ();

int main () {
    //初始设置//
    SetConsoleOutputCP(65001); //UTF-8, 保证中文不乱码
    SetConsoleCP(65001);
    set_console_font();
    lock_window();
    set_console_size(0, 0);
    hide_cursor();
    for(int i=0; i<256; ++i) DISP[i]=i; //怎么还有重载啊

    load_scores();
    load_skins();
    main_screen();

    set_color( 0, 7);
    return 0;
} //推箱子

//游戏//

void start () {
    BACK: //我重生了
    system("cls");
    //读关//
    if(!load_level()) {
        put_line( 2, 1,12,   L"你这关卡不对吧");
        put_line( 2, 2, 7, L"按任意键继续...");
        _getch();
        return;
    }
    //画图//
    set_console_size(map_W, MAP.size() +4); //上下留两行
    get_console_size(C_col, C_row);
    OX = (C_col -map_W     ) /2;
    OY = (C_row -MAP.size()) /2;
    draw_map();
    //导航//
    gotoxy((C_col -CHOSEN.size())/2, 0); set_color( 0, 7);
    printf("%s", CHOSEN.c_str());
    put_line((C_col-36)/2, C_row-2, 7, L"↑↓←→ w/s/a/d 移动 | esc 退出 | r 重开"); //36
    //游戏//
    if(run_game()) goto BACK; //结束放在里面
}

void map_change (int x, int y, int nx, int ny) { //-----修改地图
    switch(MAP[y][x]) {
    case 36: //'$'
        MAP[y][x]=32; //' '
        switch(MAP[ny][nx]) {
        case 46: MAP[ny][nx]=42; ON++; break; //'.' '*'
        case 32: MAP[ny][nx]=36; break; //' ' '$'
        } break;

    case 64: //'@'
        MAP[y][x]=32; //' '
        switch(MAP[ny][nx]) {
        case 46: MAP[ny][nx]=43; break; //'.' '+'
        case 32: MAP[ny][nx]=64; break; //' ' '@'
        } break;

    case 42: //'*'
        MAP[y][x]=46; ON--; //'.'
        switch(MAP[ny][nx]) {
        case 46: MAP[ny][nx]=42; ON++; break; //'.' '*'
        case 32: MAP[ny][nx]=36; break; //' ' '$'
        } break;

    case 43: //'+'
        MAP[y][x]=46; //'.'
        switch(MAP[ny][nx]) {
        case 46: MAP[ny][nx]=43; break; //'.' '+'
        case 32: MAP[ny][nx]=64; break; //' ' '@'
        } break;
    }
}

bool run_game () { //-----------------------------------进行游戏
    STEP=0; //MS是直接赋值的
    //开始计时//
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);
    //主循环//
    int key, dx, dy, nx, ny, bx, by; //方向 前面 前前面
    bool PUSHED;
    for(;;) {
        dx =dy =0;

        key=_getch();
        switch(key) {
        case 0: case 224: // 功能键 / 方向键
            key=_getch();
            switch(key){
            case 72:  dy=-1; break; // ↑
            case 80:  dy= 1; break; // ↓
            case 75:  dx=-1; break; // ←
            case 77:  dx= 1; break; // →
            } break;
        case 119: case 87: dy=-1; break; //'w' 'W'
        case 115: case 83: dy= 1; break; //'s' 'S'
        case  97: case 65: dx=-1; break; //'a' 'A'
        case 100: case 68: dx= 1; break; //'d' 'D'
        case 114: case 82: return 1; //'r' 'R'
        case 27:           return 0;
        }
        if(dx==0 && dy==0) continue;
        //前面//
        nx = px+dx;
        ny = py+dy;
        if(ny<0 || ny>=(int)MAP.size() || nx<0 || nx>=(int)MAP[ny].size()) continue; //防止越界
        if(MAP[ny][nx]==35 || MAP[ny][nx]==55) continue; //撞墙/边界//'#' '7'
        //前面箱子//
        PUSHED =0;
        if(MAP[ny][nx]==36 || MAP[ny][nx]==42) { //前面是箱子//'$' '*'
            bx = nx+dx;
            by = ny+dy;
            if(by<0 || by>=(int)MAP.size() || bx<0 || bx>=(int)MAP[by].size()) continue; //防止越界
            if(MAP[by][bx]==35 || MAP[by][bx]==55) continue; //箱子撞墙/边界//'#' '7'
            if(MAP[by][bx]==36 || MAP[by][bx]==42) continue; //箱子顶箱子//'$' '*'
            PUSHED =1;
        }
        if(PUSHED) {
            map_change(nx, ny, bx, by); //推!
            draw_cell(bx, by);
        }
        //玩家移动//
        map_change(px, py, nx, ny); //走!
        draw_cell(nx, ny); draw_cell(px, py);
        px =nx;
        py =ny;
        STEP++;
        //判断箱子归位//
        if(ON==TARGET) {
            QueryPerformanceCounter(&end); //结束计时
            MS =(double)(end.QuadPart - start.QuadPart) /frequency.QuadPart *1000;

            set_color( 0, 7); //防止背景色
            set_console_size(0, 0); //复原
            return end_screen();
        }
    }
}

//绘制//

void draw_cell (int x, int y) { //----------------------画一格
    gotoxy(x+OX, y+OY);
    switch(MAP[y][x]) {
        case 36: set_color( 0, 6); fputs(DISP[36].c_str(), stdout); break; //'$' //没归位:暗黄
        case 64: set_color( 0,11); fputs(DISP[64].c_str(), stdout); break; //'@' //玩家:青
        case 46: set_color( 0,12); fputs(DISP[46].c_str(), stdout); break; //'.' //目标:红
        case 42: set_color( 0,10); fputs(DISP[42].c_str(), stdout); break; //'*' //归位:绿
        case 43: set_color( 0, 3); fputs(DISP[43].c_str(), stdout); break; //'+' //玩家在目标:暗青
        case 35: set_color( 0,15); fputs(DISP[35].c_str(), stdout); break; //'#' //墙
        case 55: set_color( 0, 8); fputs(DISP[55].c_str(), stdout); break; //'7' //边界
        case 32: set_color( 0, 0); putchar(32);                     break; //' ' //空
    }
}

void draw_map () { //-----------------------------------绘制整个地图
    for(size_t y=0; y<MAP.size(); y++) for(size_t x=0; x<MAP[y].size(); x++)
        draw_cell(x, y);
}

//界面//

void main_screen () { //--------------------------------主界面
    for(;;) {
        BACK:
        system("cls");
        put_line(14, 4, 7, L"╔════════════════════════╗");
        put_line(14, 5, 7, L"║        ┌──────┐        ║");
        put_line(14, 6, 7, L"║        │ 菜单 │        ║");
        put_line(14, 7, 7, L"║        └──────┘        ║");
        put_line(14, 8, 7, L"║        1. 选关         ║");
        put_line(14, 9, 7, L"║        2. 皮肤         ║");
        put_line(14,10, 7, L"╟──────────┬──┬──────────╢");
        put_line(14,11, 7, L"║ 数字选择 │77│ esc 退出 ║");
        put_line(14,12, 7, L"╚══════════╧══╧══════════╝");
        put_line(23, 5,10,          L"┌──────┐");
        put_line(23, 6,10,          L"│ 菜单 │");
        put_line(23, 7,10,          L"└──────┘");
        put_line(23, 8,14,          L"1.");
        put_line(23, 9,14,          L"2.");
        put_line(26, 8,11,             L"选关");
        put_line(26, 9,11,             L"皮肤");
        put_line(16,11,10,   L"数字选择");
        put_line(30,11,10,                 L"esc 退出");
        int key;
        for(;;) {
            key=_getch();
            switch(key) {
            case 27: return;
            case 49: choose_screen(); goto BACK; //'1'
            case 50:   skin_screen(); goto BACK; //'2'
            }
        }
    }
}

void choose_screen () { //------------------------------选关界面
    find_level();
    //没有关卡//
    if(LEVELS.empty()) {
        system("cls");
        put_line( 2, 7,12,   L"我关卡文件呢?");
        put_line( 2, 8, 7,   L"levels里怎么啥也没有?");
        put_line( 0, 9, 7, L"按任意键继续...");
        _getch();
        return;
    }

    int key;
    size_t page=1, id, i; //页码, 本页最大下标+1, 循环下标
    BACKBACK:
    system("cls");
    put_line( 2, 1, 7, L"╔════════════════════════════════════════════════╗");
    put_line( 2, 2, 7, L"║                   ┌────────┐                   ║");
    put_line( 2, 3, 7, L"║                   │ 选关卡 │                   ║");
    put_line( 2, 4, 7, L"║                   └────────┘                   ║");
    put_line( 2, 5, 7, L"║ 1. level_1   ————: 7777 ——  ———— 77777.777 ——  ║");
    put_line( 2, 6, 7, L"║ 2. level_2   ————: 7777 ——  ———— 77777.777 ——  ║");
    put_line( 2, 7, 7, L"║ 3. level_3   ————: 7777 ——  ———— 77777.777 ——  ║");
    put_line( 2, 8, 7, L"║ 4. level_4   ————: 7777 ——  ———— 77777.777 ——  ║");
    put_line( 2, 9, 7, L"║ 5. level_5   ————: 7777 ——  ———— 77777.777 ——  ║");
    put_line( 2,10, 7, L"║ 6. level_6   ————: 7777 ——  ———— 77777.777 ——  ║");
    put_line( 2,11, 7, L"║ 7. level_7                     ——————————————  ║");
    put_line( 2,12, 7, L"║   ←↑上一页        第 7777 页        下一页↓→   ║");
    put_line( 2,13, 7, L"╟──────────┬───────┬──────────┬───────┬──────────╢");
    put_line( 2,14, 7, L"║ 数字选关 │ 7 7 7 │ 方向换页 │ 7 7 7 │ esc 退出 ║");
    put_line( 2,15, 7, L"╚══════════╧═══════╧══════════╧═══════╧══════════╝");
    put_line(24, 3,10,                       L"选关卡");
    put_line( 4,14,10,   L"数字选关");
    put_line(23,14,10,                      L"方向换页");
    put_line(42,14,10,                                         L"esc 退出");
    BACK:
    //页码//
    if(page==0) page =(LEVELS.size() +6) /7; //越下界
    if(LEVELS.size() < page*7 -6) page =1;   //越上界
    gotoxy(25,12); set_color( 0, 7);
    printf("%4zu", page);
    //本页关卡//
    id = page*7 > LEVELS.size()? LEVELS.size() : page*7; //上界+1
    for(i = page*7 -7; i<id; i++) {
        gotoxy( 4, i%7 +5);
        //编号与名字//
        set_color( 0,14); printf("%d. ", (int)(i%7 +1));
        set_color( 0, 7); printf("%-9s", LEVELS[i].c_str());
        //历史最佳//
        map<string, pair<int, int> >::iterator it =BESTS.find(LEVELS[i]);
        if(it != BESTS.end()) {
            set_color( 0, 3); printf("  最佳: ");
            set_color( 0,11); printf("%4d", it->second.first);
            set_color( 0, 3); printf(" 步  用时 ");
            set_color( 0,11); printf("%9.3lf", it->second.second /1000.0);
            set_color( 0, 3); printf(" 秒");
        } else {
            set_color( 0, 8); printf("                   还没有人征服过");
        }
    }
    for(i=id; i<page*7; ++i) { //没满
        put_line( 2, i%7 +5, 7, L"║                                                ║");
    }
    //按键//
    for(;;) {
        key=_getch();
        switch(key) {
        case 0: case 224: //方向键
            key=_getch();
            switch(key){
            case 72: case 75: page--; break;
            case 77: case 80: page++; break;
            }
            goto BACK;
        case 27: return;
        }
        if(49<=key && key<=55) //'1'-'7'
            if(page*7 +key -56 < id) { //我已化简
                CHOSEN = LEVELS[page*7 +key -56];
                start();
                goto BACKBACK;
            }
    }
}

void skin_screen () {
    //方向选择
    //-> 7 :
    //显示当前皮肤
    //esc 返回
}

bool end_screen () { //---------------------------------通关画面
    system("cls");
    put_line( 2, 1,10, L"================================================");
    put_line(14, 2,14,             L"* * *");
    put_line(21, 2,12,                    L"你 过 关 !");
    put_line(33, 2,14,                                L"* * *");
    put_line( 2, 3,10, L"================================================");
    put_line(12, 5,14,           L"*    *    *    *    *    *");
    put_line(10, 6,14,         L"*      Y O U   W I N ! !     *");
    put_line(12, 7,14,           L"*    *    *    *    *    *");
    put_line( 3, 9, 7,   L"通关总耗时: ");
    put_line( 5,10, 7,     L"消耗步数: ");
    put_line( 5,12, 7,     L"历史最佳: ");
    put_line( 1,14, 7, L"按'esc'返回选关 | 按'r'重开");
    //耗时//
    gotoxy(14, 9); set_color( 0,11);
    printf("        %9.3lf 秒", MS/1000.0);
    //步数//
    gotoxy(14,10); set_color( 0,11);
    printf("%4d 步", STEP);
    //最佳//
    gotoxy(14,12);
    if(save_score()) {
        set_color( 0,10); printf("%4d 步 %9.3lf 秒 新纪录! 裱起来!", STEP, MS/1000.0);
    } else {
        set_color( 0,11); printf("%4d 步 %9.3lf 秒", BESTS[CHOSEN].first, BESTS[CHOSEN].second/1000.0);
    }
    //闪烁+退出//
    int key;
    for(;;) {
        put_line(17, 6, Rand_color(), L"Y O U   W I N ! !");
        Sleep(100);
        if(_kbhit()) {
            key=_getch();
            switch(key) {
            case 27:           return 0;
            case 82: case 114: return 1; //'R' 'r'
            }
        }
    }
}

//存取//

void load_scores () { //--------------------------------读历史最佳
    BESTS.clear();

    ifstream in(SCORE);
    if(!in.is_open()){
        put_line( 3, 0,12,   L"出了点小问题, 成绩读取失败了");
        put_line( 3, 1,14,   L"但是并不影响你玩, 对吗?");
        put_line( 0, 2, 7, L"按任意键继续...");
        _getch();
        return;
    }
    string name;
    int step, ms;
    while(in>>name>>step>>ms)
        BESTS[name].first  =step,
        BESTS[name].second =ms  ;
}

bool save_score () { //---------------------------------保存,返回是否新纪录
    map<string, pair<int, int> >::iterator it=BESTS.find(CHOSEN);
    if(it!=BESTS.end()) {
        if(it->second.first<STEP)                           return 0; //步数更差
        if(it->second.first==STEP && it->second.second<=MS) return 0; //秒数不小
    }
    BESTS[CHOSEN].first  =STEP;
    BESTS[CHOSEN].second =MS  ;
    ofstream out(SCORE+".tmp"); //临时文件,防止写到一半停了
    for(it=BESTS.begin(); it!=BESTS.end(); ++it)
        out <<it->first        <<" "
            <<it->second.first <<" "
            <<it->second.second<<"\n";
    out.close(); //防止阻碍合并
    error_code ec;
    filesystem::rename(SCORE+".tmp", SCORE, ec); //合并
    if(ec){ //有错误信息
        put_line( 3, 0,12,   L"emmm, 坏了, 保存失败了");
        put_line( 3, 1,14,   L"哎呀不管了, 先祝贺你再说");
        put_line( 0, 2, 7, L"按任意键继续...");
        _getch();
    }
    return 1;
}

void load_skins () {
    //失败
    //加载
}

bool load_level () { //---------------------------------读关卡文件
    MAP.clear();
    map_W=0;

    ifstream in(DIR + CHOSEN + EXTENTION); //选的关卡路径
    if(!in.is_open()) return 0;
    //读地图//
    string line;
    while(getline(in, line)) {
        if(!line.empty() && line[line.size()-1]==13) line.erase(line.size()-1); //去\r
        if( line.empty()) break; //空行
        MAP.push_back(line);
        map_W = map_W < line.size()? line.size() : map_W;
    }
    //找箱子/玩家/目标//
    px =py =-1;
    ON =TARGET =0;
    int BOX=0, PLAYER=0;
    for(size_t y=0; y<MAP.size(); y++) for(size_t x=0; x<MAP[y].size(); x++) {
        switch(MAP[y][x]) {
            case 36: BOX++;                          break; //'$'
            case 64: px=x; py=y; PLAYER++;           break; //'@'
            case 46:                       TARGET++; break; //'.'
            case 42: BOX++;      ON++;     TARGET++; break; //'*' //箱子在目标点上
            case 43: px=x; py=y; PLAYER++; TARGET++; break; //'+' //玩家在目标点上
            case 35: case 55: case 32: break; //'#' '7' ' '
            default: return 0; //非法字符
        }
    }
    if(MAP.empty() || px<0 || TARGET>BOX || !TARGET || PLAYER!=1) return 0;
    return 1;
}

//关卡//

string base_name (string s) { //------------------------去掉路径,只留文件名
    size_t p = s.find_last_of("\\/");
       s = p==string::npos? s : s.substr( p+1);
           p = s.find_last_of(".");
    return p==string::npos? s : s.substr(0, p);
}

int level_num (const string& s) { //--------------------从文件名里抠出关卡号
    int n=0;
    for(size_t i=0; i<s.size(); i++)
        if(s[i]>=48 && s[i]<=57) n = n*10 +s[i] -48; //'0' '9'
    return n;
}

void find_level () { //---------------------------------扫描关卡文件夹
    LEVELS.clear();

    WIN32_FIND_DATAA data;
    HANDLE HFFF =FindFirstFileA((DIR + NAME + EXTENTION).c_str(), &data); //超级拼装
    if(HFFF!=INVALID_HANDLE_VALUE) {
        do { LEVELS.push_back(base_name(data.cFileName)); } //存
        while(FindNextFileA(HFFF, &data));
        //搜完了//
        FindClose(HFFF);
    }
    for(size_t i=0; i<LEVELS.size(); i++) for(size_t j=i+1; j<LEVELS.size(); j++)
            if(level_num(LEVELS[i]) > level_num(LEVELS[j])) swap(LEVELS[i], LEVELS[j]); //排序
}

//小插件//

bool is_wide (wchar_t c) { //---------------------------判断字符是否占两格
    WORD t=0;
    GetStringTypeW(CT_CTYPE3, &c, 1, &t);
    return (t & (C3_FULLWIDTH|C3_IDEOGRAPH)) != 0;
}

int disp_width (wstring s) { //-------------------------测宽字符串长度
    int w=0;
    for(wchar_t c : s) {
        //通过获取字符属性累计//
        WORD t=0;
        GetStringTypeW(CT_CTYPE3, &c, 1, &t);
        w += (t & (C3_FULLWIDTH|C3_IDEOGRAPH)) ? 2 : 1;
    }
    return w;
}

void put_line (SHORT x, SHORT y, WORD A, wstring s) { //按位置输出一行带颜色字符串
    int w=disp_width(s);
    vector<CHAR_INFO> Buf(w);
    int cx=0; //屏幕列位置
    for(size_t i=0; i<s.size(); i++) {
        if(is_wide(s[i]) && cx+1<w) { //宽字符占两格, 两格都要标 LVB
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
    WriteConsoleOutputW(H_out, Buf.data(), size, Buf_Org, &region);
}

//控制台//

void set_console_font () { //---------------------------改字体
    CONSOLE_FONT_INFOEX C_font = {};
    C_font.cbSize       = sizeof(C_font);
    C_font.dwFontSize.Y = 16;
    wcscpy_s(C_font.FaceName, L"Lucida Console");
    SetCurrentConsoleFontEx(H_out, false, &C_font);
}

void lock_window () { //--------------------------------不让动窗口大小
    HWND     C_W  = GetConsoleWindow();
    LONG_PTR W_LP = GetWindowLongPtrA(C_W, GWL_STYLE);
    SetWindowLongPtrA(C_W, GWL_STYLE, W_LP & ~WS_THICKFRAME);
}

void set_console_size (SHORT W, SHORT H) { //-----------设窗口和缓冲区大小
    //规范W,H//
    if(W<min_W) W=min_W;
    if(H<min_H) H=min_H;
    COORD     C_W_max = GetLargestConsoleWindowSize(H_out);
    if(W> C_W_max.X) W=C_W_max.X;
    if(H> C_W_max.Y) H=C_W_max.Y;
    //放大缓冲区//
    CONSOLE_SCREEN_BUFFER_INFO Buf_info;
    GetConsoleScreenBufferInfo(H_out, &Buf_info);
    COORD    Buf_size = { Buf_info.dwSize.X <W ? W : Buf_info.dwSize.X,
                          Buf_info.dwSize.Y <H ? H : Buf_info.dwSize.Y };
    SetConsoleScreenBufferSize(H_out, Buf_size);
    //设窗口大小//
    SMALL_RECT W_size = { 0, 0, SHORT(W-1), SHORT(H-1) };
    SetConsoleWindowInfo(H_out, true, &W_size);
    SetConsoleScreenBufferSize(H_out, {W, H}  ); //缩缓冲区到没有滚动条
}

void hide_cursor () { //--------------------------------隐藏光标
    CONSOLE_CURSOR_INFO Cur_info;
    Cur_info.bVisible =false;
    Cur_info.dwSize   =1;
    SetConsoleCursorInfo(H_out, &Cur_info);
}

void gotoxy (SHORT x, SHORT y) { //---------------------移动光标
    COORD Cur_pos={x, y};
    SetConsoleCursorPosition(H_out, Cur_pos);
}

void get_console_size (auto& col, auto& row) { //-------获取控制台宽高
    CONSOLE_SCREEN_BUFFER_INFO Buf_info;
    GetConsoleScreenBufferInfo(H_out, &Buf_info);
    col = Buf_info.srWindow.Right  - Buf_info.srWindow.Left +1;
    row = Buf_info.srWindow.Bottom - Buf_info.srWindow.Top  +1;
}

void set_color (int BG, int FG) { //--------------------设置颜色, BG背景, FG前景
    SetConsoleTextAttribute(H_out, (BG<<4)|FG);
}

//随机函数//

inline int Rand_color () { //---------------------------随机颜色
    return RAND_COLOR(RNG);
}

/*依旧学习
1.ifstream   ofstream
2.文件搜索
3.do{}while ();
4.set存变量
5.比较size()最好用size_t
6.for里用++i更好
7.filesystem文件操作
*/
