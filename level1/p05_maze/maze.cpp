#include<iostream>
#include<stack>
#include<random>
#include<conio.h>
#include<windows.h>
using namespace std;

random_device RD;
mt19937 RNG(RD());
uniform_int_distribution<int> RAND(0, 11); //2,3,4的公倍数12
uniform_int_distribution<int> RAND_COLOR(1, 15); //1-15随机颜色,避开黑色
inline int Rand ();
inline int Rand_color ();

HANDLE H_out = GetStdHandle (STD_OUTPUT_HANDLE); //控制台输出句柄
CONSOLE_SCREEN_BUFFER_INFO Buf_info; //------------缓冲区信息
SHORT C_row, C_col; //-----------------------------控制台大小
const int min_W=52; //-----------------------------最小宽度(选色52列)
const int min_H=14; //-----------------------------最小高度(设置12行+上下空)
void set_console_font ();
void lock_window ();
void unlock_window ();
void get_console_size (auto& col, auto& row);
void set_console_size (SHORT W, SHORT H);
void erase_line (SHORT y);
void hide_cursor ();
void gotoxy (SHORT x, SHORT y);
void set_color (int BG, int FG);

const char* COLOR_NAMES[16]= //---颜色模版
{"黑色", "深蓝", "深绿", "青色", "深红", "紫色", "暗黄", "浅灰", "深灰", "亮蓝", "亮绿", "亮青", "亮红", "亮紫", "黄色", "白色"};
// 0      1      2      3     4      5      6      7     8      9     10     11     12     13    14     15  //

int OX, OY; //--------------------偏移量
void main_screen ();
void set_screen ();
void end_screen ();

bool is_wide (wchar_t c);
int  disp_width (wstring s);
void put_line (SHORT x, SHORT y, WORD A, wstring s);

char player='7';
int player_A=10, wall_A=7, BG_A=0;
void pick_symbol ();
void show_color_table ();
void pick_color (int&A, const string& T, int Def);
void set_maze_size ();

int maze_W=27, maze_H=27; //------地图宽高
vector<string> maze;
void generate_maze ();
void draw_maze ();

double MS; //---------------------计时
void start ();
bool run_game ();

int main () {
    //初始化//
    SetConsoleOutputCP (65001); //UTF-8, 保证中文不乱码
    SetConsoleCP (65001);
    set_console_font ();
    set_console_size (0, 0);
    get_console_size (C_col, C_row);
    lock_window ();
    hide_cursor ();

    main_screen ();

    return 0;
} //随机迷宫+一堆功能

//游戏中//

void start () { //--------------------------------------开始
    system ("cls");

    maze.clear();
    generate_maze ();

    set_console_size (C_col, C_row);
    draw_maze ();
    //底部导航//
    gotoxy ((C_col-45)/2, C_row-2);
    set_color (BG_A, wall_A);
    printf ("↑↓←→ w/s/a/d 移动 | esc 退出 | S 入口  E 出口 "); //45

    bool o=run_game ();

    set_color ( 0, 7); //防止背景色
    set_console_size (0, 0); //复原

    if (o) end_screen ();
}

void generate_maze () { //------------------------------生成迷宫
    //算大小//
    int room_col =(maze_W -3) /2;
    int room_row =(maze_H -3) /2; //房间行列数
          maze_W = 2*room_col +3;
          maze_H = 2*room_row +3; //网格比(2n+1)再多一圈
    //搜索状态//
    vector< vector<bool> > vis(room_row, vector<bool>(room_col, 0));
    vis[0][0]=1; //vis==1: 房间在队列
    stack< pair<int, int> > st; //记录房间坐标
    st.push(make_pair (0, 0));
    //辅助//
    int R, C, NR, NC, n, cand[4], d;
    const int DR[4] ={-1, 1, 0, 0};
    const int DC[4] ={ 0, 0, -1, 1}; //偏移
    //地图初始化//
    maze.assign(maze_H, string(maze_W, '#'));
    maze[2][2]=' ';
    //DFS 回溯法, 保证全联通//
    while (!st.empty()) {
        R =st.top().first;
        C =st.top().second;
        n=0;
        for (size_t i=0; i<4; i++) {
            NR = R+DR[i];
            NC = C+DC[i];
            if (NR>=0 && NR<room_row && NC>=0 && NC<room_col && !vis[NR][NC])
                cand[n++]=i; //存候选挖墙方向
        }
        if (n==0) { st.pop(); continue; }

        d  = cand[Rand ()%n]; //随机选一个方向
        NR = R+DR[d];
        NC = C+DC[d];
        maze[2*R +2 +DR[d]][2*C +2 +DC[d]]=' ';
        maze[2*NR +2][2*NC +2]=' '; //挖墙(注意坐标换算)
        vis[NR][NC]=1;
        st.push(make_pair (NR, NC)); //新房间
    }
    //神秘捷径//
    for (int x=0; x<maze_W; x++) { maze[0][x]='7'; maze[maze_H-1][x] ='7'; }
    for (int y=0; y<maze_H; y++) { maze[y][0]='7'; maze[y][maze_W-1] ='7'; }

    maze[1][2]              ='S'; //入口
    maze[maze_H-2][maze_W-3]='E'; //出口
}

void draw_maze () { //----------------------------------绘制迷宫
    for (int y=0; y<maze_H; y++) {
        gotoxy (0+OX, y+OY);
        for (int x=0; x<maze_W; x++)
            switch (maze[y][x]) {
            case 35: set_color (BG_A, wall_A); fputs ("█", stdout); break; //'#'
            case 55: set_color (BG_A, wall_A); fputs ("▒", stdout); break; //'7'
            case 32: set_color (BG_A,  BG_A); putchar (32);      break; //' '
            case 83: set_color (BG_A,    10); putchar (83);      break; //'S'
            case 69: set_color (BG_A,    12); putchar (69);      break; //'E'
            }
    }
}

bool run_game () { //-----------------------------------运行游戏
    //玩家放到入口//
    int px=2, py=1;
    gotoxy (px+OX, py+OY);
    set_color (BG_A, player_A);
    putchar (player);
    //开始计时//
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency (&frequency);
    QueryPerformanceCounter (&start);
    //辅助//
    int key, dx, dy, nx, ny;
    //运动和打印//
    for (;;) {
        key=_getch ();
        if (key==27) return 0;
        //方向//
        dx=0, dy=0;
        switch (key) {
        case 119: case 87: dy=-1; break; //'w' 'W'
        case 115: case 83: dy= 1; break; //'s' 'S'
        case  97: case 65: dx=-1; break; //'a' 'A'
        case 100: case 68: dx= 1; break; //'d' 'D'
        case 0: case 224:
            key=_getch ();
            switch (key) {
            case 72: dy=-1; break; // ↑
            case 80: dy= 1; break; // ↓
            case 75: dx=-1; break; // ←
            case 77: dx= 1; break; // →
            }
        }
        if (dx==0 && dy==0) continue;
        //前方//
        nx = px+dx;
        ny = py+dy;
        if (nx<0 || ny<0 || nx>=maze_W || ny>=maze_H) continue; //出界
        if (maze[ny][nx]==35) continue; //撞墙 //'#'
        //打印原处//
        gotoxy (px+OX, py+OY);
        switch (maze[py][px]) {
        case 83: set_color (BG_A,    10); putchar (83);      break; //'S'
        case 55: set_color (BG_A, wall_A); fputs ("▒", stdout); break; //'7'
        case 32: set_color (BG_A,  BG_A); putchar (32);      break; //' '
        }
        //打印玩家//
        px =nx;
        py =ny;
        gotoxy (px+OX, py+OY);
        set_color (BG_A, player_A);
        putchar (player);
        //到终点//
        if (maze[py][px]==69) { //'E'
            QueryPerformanceCounter (&end); //结束计时
            MS=(double)(end.QuadPart-start.QuadPart)/frequency.QuadPart*1000;
            return 1;
        }
    }
}

//小插件//

bool is_wide (wchar_t c) { //---------------------------判断字符是否占两格
    WORD t=0;
    GetStringTypeW (CT_CTYPE3, &c, 1, &t);
    return (t & (C3_FULLWIDTH|C3_IDEOGRAPH)) != 0;
}

int disp_width (wstring s) { //-------------------------测宽字符串长度
    int w=0;
    for (wchar_t c : s) {
        //通过获取字符属性累计//
        WORD t=0;
        GetStringTypeW (CT_CTYPE3, &c, 1, &t);
        w += (t & (C3_FULLWIDTH|C3_IDEOGRAPH)) ? 2 : 1;
    }
    return w;
}

void put_line (SHORT x, SHORT y, WORD A, wstring s) { //按位置输出一行带颜色字符串
    int w=disp_width (s);
    vector<CHAR_INFO> Buf(w);
    int cx=0; //屏幕列位置
    for (size_t i=0; i<s.size(); i++) {
        if (is_wide (s[i]) && cx+1<w) { //宽字符占两格, 两格都要标 LVB
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
    WriteConsoleOutputW (H_out, Buf.data(), size, Buf_Org, &region);
}

//设置//

void pick_symbol () { //--------------------------------选角
    system ("cls");
    for (;;) {
        put_line ( 2, 1, 7,  L"请输入一个常见字符作为玩家符号 (回车默认'7'):");
        set_color ( 0, 7); //put_line不改控制台当前属性, 输入要按这个色显示
        gotoxy ( 0, 2);
        string s;
        getline (cin, s);
        if ( s.empty() ) { player ='7' ; return; }
        if (s.size()==1) { player =s[0]; return; }
        put_line ( 2, 3, 7,  L"你...我...叫你输");
        put_line (19, 3,12,                   L"一个! 常见的! ");
        erase_line (2);
    }
}

void show_color_table () { //---------------------------展示颜色选择
    for (int i=0; i<16; i++) {
        set_color ( 0, 7); printf ("  %2d:", i);
        set_color ( 0, i); printf ("%s", COLOR_NAMES[i]);
        if (i%4==3) putchar (10); //'\n'
        else printf ("     ");
    }
}

void pick_color (int&A, const string& T, int Def) { //--选颜色
    system ("cls");
    for (;;) {
        set_color ( 0,  7); printf ("\n-------请选择%s(0-15,回车默认%d:", T.c_str(), Def);
        set_color ( 0, Def); printf ("%s", COLOR_NAMES[Def]);
        set_color ( 0,  7); printf (")-------\n");
        show_color_table ();
        put_line ( 0, 6, 7, L"请输入编号: ");
        set_color ( 0, 7); //put_line不改控制台当前属性, 输入要按这个色显示
        gotoxy (12, 6);
        string s;
        getline (cin, s);
        if ( s.empty() ) { A =Def; return; }
        int n=atoi (s.c_str()); //转换成数字
        if (n>=0&&n<=15) { A = n ; return; }
        put_line ( 2, 7, 7,  L"你瞎吗? ");
        put_line (10, 7,12,          L"0-15");
        put_line (14, 7, 7,              L"看不见?");
        erase_line (6);
        gotoxy ( 0, 0);
    }
}

void set_maze_size () { //------------------------------设置迷宫大小
    BACK:
    system ("cls");
    unlock_window ();
    put_line ( 2, 1, 7,  L"请调整你的窗口大小以决定迷宫大小");
    put_line ( 0, 2, 8, L"按'enter'结束调整");
    for (;;) if (_getch()==13) break;
    //读取调整//
    lock_window ();
    get_console_size (maze_W, maze_H);
    if (maze_W<3 || maze_H<3) {
        system ("cls");
        put_line ( 2, 1,12,  L"这么小的迷宫你玩啥呢");
        put_line ( 0, 2, 7, L"按任意键继续...");
        _getch ();
        goto BACK;
    }
    set_console_size (maze_W, maze_H+2);
    get_console_size (C_col, C_row);
    //计算偏移//
    OX =(C_col -maze_W) /2   ;
    OY =(C_row -maze_H) /2 -1;
    //复原//
    set_console_size (0, 0);
}

//界面//

void main_screen () { //--------------------------------主界面
    for (;;) {
        BACK:
        system ("cls");
        put_line ( 2, 1, 7,  L"╔════════════════════════╗");
        put_line ( 2, 2, 7,  L"║        ┌──────┐        ║");
        put_line ( 2, 3, 7,  L"║        │ 菜单 │        ║");
        put_line ( 2, 4, 7,  L"║        └──────┘        ║");
        put_line ( 2, 5, 7,  L"║        1. 开始         ║");
        put_line ( 2, 6, 7,  L"║        2. 设置         ║");
        put_line ( 2, 7, 7,  L"╟──────────┬──┬──────────╢");
        put_line ( 2, 8, 7,  L"║ 数字选择 │77│ esc 退出 ║");
        put_line ( 2, 9, 7,  L"╚══════════╧══╧══════════╝");
        put_line (11, 2,10,           L"┌──────┐");
        put_line (11, 3,10,           L"│ 菜单 │");
        put_line (11, 4,10,           L"└──────┘");
        put_line (11, 5,14,           L"1.");
        put_line (11, 6,14,           L"2.");
        put_line (14, 5,11,              L"开始");
        put_line (14, 6,11,              L"设置");
        put_line ( 4, 8,10,    L"数字选择");
        put_line (18, 8,10,                  L"esc 退出");
        int key;
        for (;;) {
            key=_getch ();
            switch (key) {
            case 27 : return;
            case 49: start ();      goto BACK; //'1'
            case 50: set_screen (); goto BACK; //'2'
            }
        }
    }
}

void set_screen () { //---------------------------------设置界面
    for (;;) {
        BACK:
        system ("cls");
        put_line ( 2, 1, 7,  L"╔════════════════════════╗");
        put_line ( 2, 2, 7,  L"║        ┌──────┐        ║");
        put_line ( 2, 3, 7,  L"║        │ 设置 │        ║");
        put_line ( 2, 4, 7,  L"║        └──────┘        ║");
        put_line ( 2, 5, 7,  L"║      1. 角色符号       ║");
        put_line ( 2, 6, 7,  L"║      2. 角色颜色       ║");
        put_line ( 2, 7, 7,  L"║      3. 墙壁颜色       ║");
        put_line ( 2, 8, 7,  L"║      4. 背景颜色       ║");
        put_line ( 2, 9, 7,  L"║      5. 地图大小       ║");
        put_line ( 2,10, 7,  L"╟──────────┬──┬──────────╢");
        put_line ( 2,11, 7,  L"║ 数字选择 │77│ esc 退出 ║");
        put_line ( 2,12, 7,  L"╚══════════╧══╧══════════╝");
        put_line (11, 2,10,           L"┌──────┐");
        put_line (11, 3,10,           L"│ 设置 │");
        put_line (11, 4,10,           L"└──────┘");
        put_line ( 9, 5,14,         L"1.");
        put_line ( 9, 6,14,         L"2.");
        put_line ( 9, 7,14,         L"3.");
        put_line ( 9, 8,14,         L"4.");
        put_line ( 9, 9,14,         L"5.");
        put_line (12, 5,11,            L"角色符号");
        put_line (12, 6,11,            L"角色颜色");
        put_line (12, 7,11,            L"墙壁颜色");
        put_line (12, 8,11,            L"背景颜色");
        put_line (12, 9,11,            L"地图大小");
        put_line ( 4,11,10,    L"数字选择");
        put_line (18,11,10,                  L"esc 退出");
        int key;
        for (;;) {
            key=_getch ();
            switch (key) {
            case 27 : return;
            case 49: pick_symbol (); goto BACK; //'1'
            case 50: pick_color (player_A, "角色的颜色", 10); goto BACK; //'2'
            case 51: pick_color (  wall_A, "墙壁的颜色", 7); goto BACK; //'3'
            case 52: pick_color (    BG_A, "背景的颜色", 0); goto BACK; //'4'
            case 53: set_maze_size (); goto BACK; //'5'
            }
        }
    }
}

void end_screen () { //---------------------------------通关画面
    system ("cls");
    //计算时间//
    int    m = MS/60000;
    double s = (MS - m*60000) /1000.0;

    put_line ( 1, 1,10, L"================================================");
    put_line ( 4, 2,14,    L"* * *                                * * *");
    put_line (11, 2,12,           L"恭 喜 你 成 功 走 出 迷 宫 !");
    put_line ( 1, 3,10, L"================================================");
    put_line (12, 4,14,            L"*    *    *    *    *    *");
    put_line (10, 5,14,          L"*     Y O U    W I N ! !     *");
    put_line (12, 6,14,            L"*    *    *    *    *    *");
    put_line (12, 7,14,            L"通关总耗时: ");
    gotoxy (24, 7);
    set_color ( 0,11);
    if (m>0) printf ("%d 分 %.4lf 秒\n", m, s);
    else    printf (      "%.4lf 秒\n",    s);
    put_line ( 0, 8, 7, L"按'enter'继续...");
    //闪烁+退出//
    for (;;) {
        put_line (16, 5, Rand_color(),                L"Y O U    W I N ! !");
        Sleep (150);
        if (_kbhit()) if (_getch()==13) break;
    }
}

//控制台工具//

void set_console_font () { //---------------------------改字体
    CONSOLE_FONT_INFOEX C_font = {};
    C_font.cbSize       = sizeof(C_font);
    C_font.dwFontSize.Y = 16;
    wcscpy_s (C_font.FaceName, L"Lucida Console");
    SetCurrentConsoleFontEx (H_out, FALSE, &C_font);
}

void lock_window () { //--------------------------------不让动窗口大小
    HWND     C_W   = GetConsoleWindow ();
    LONG_PTR W_LP  = GetWindowLongPtrA (C_W, GWL_STYLE);
    SetWindowLongPtrA (C_W, GWL_STYLE, W_LP & ~WS_THICKFRAME);
}

void unlock_window () { //------------------------------解除窗口锁定
    //放大缓冲区//
    SetConsoleScreenBufferSize (H_out, GetLargestConsoleWindowSize (H_out));
    
    HWND     C_W   = GetConsoleWindow ();
    LONG_PTR W_LP  = GetWindowLongPtrA (C_W, GWL_STYLE);
    SetWindowLongPtrA (C_W, GWL_STYLE, W_LP | WS_THICKFRAME);
}

void get_console_size (auto& col, auto& row) { //-------获取控制台宽高
    GetConsoleScreenBufferInfo (H_out, &Buf_info);
    col = Buf_info.srWindow.Right  - Buf_info.srWindow.Left +1;
    row = Buf_info.srWindow.Bottom - Buf_info.srWindow.Top  +1;
}

void set_console_size (SHORT W, SHORT H) { //-----------设窗口和缓冲区大小
    //规范W,H//
    if (W<min_W) W=min_W;
    if (H<min_H) H=min_H;
    COORD C_W_max     = GetLargestConsoleWindowSize (H_out);
    if (W> C_W_max.X) W=C_W_max.X;
    if (H> C_W_max.Y) H=C_W_max.Y;
    //放大缓冲区//
    GetConsoleScreenBufferInfo (H_out, &Buf_info);
    COORD Buf_size    = { Buf_info.dwSize.X <W ? W : Buf_info.dwSize.X,
                          Buf_info.dwSize.Y <H ? H : Buf_info.dwSize.Y };
    SetConsoleScreenBufferSize (H_out, Buf_size);
    //设窗口大小//
    SMALL_RECT W_size = { 0, 0, SHORT(W-1), SHORT(H-1) };
    SetConsoleWindowInfo (H_out, TRUE, &W_size); //放大缓冲区
    SetConsoleScreenBufferSize (H_out, {W, H}  ); //缩缓冲区到没有滚动条
}

void erase_line (SHORT y) { //--------------------------擦除指定行
    COORD pos={0, y}; DWORD x;
    FillConsoleOutputCharacter (H_out, ' ', C_col, pos, &x);
}

void hide_cursor () { //--------------------------------隐藏光标
    CONSOLE_CURSOR_INFO Cur_info;
    Cur_info.bVisible =FALSE;
    Cur_info.dwSize   =1;
    SetConsoleCursorInfo (H_out, &Cur_info);
}

void gotoxy (SHORT x, SHORT y) { //---------------------移动光标
    COORD Cur_pos={x, y};
    SetConsoleCursorPosition (H_out, Cur_pos);
}

void set_color (int BG, int FG) { //--------------------设置颜色, BG背景, FG前景
    SetConsoleTextAttribute (H_out, (BG<<4)|FG);
}

//随机函数//

inline int Rand () { //---------------------------------随机数
    return RAND (RNG);
}

inline int Rand_color () { //---------------------------随机颜色
    return RAND_COLOR (RNG);
}

/*
学习成果
1.static +
2.const + 
3.获取控制台宽高可以写一起
4.注释也可以这么美丽
5.对齐可视化
6.{"","",... }依次返回存储""字符常量的首地址
7.&引用, 本质也是指针
8.for(;;)
9.fflush(stdout)更新画面
10.unsigned char
11.string(n,ch)生成
12.超强随机函数
*/
