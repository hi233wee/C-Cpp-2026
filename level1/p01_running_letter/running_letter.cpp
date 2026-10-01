#include<iostream>
#include<string>
#include<cstdio>
#include<vector>
#include<windows.h>
#include<conio.h>
using namespace std;

#define sleeptime 50

HANDLE H_out = GetStdHandle(STD_OUTPUT_HANDLE);//控制台输出句柄
CONSOLE_SCREEN_BUFFER_INFO Buf_info;//-----------缓冲区信息
SHORT C_row, C_col;//----------------------------控制台大小
void lock_window();
void set_console_size(SHORT W, SHORT H);
void get_console_size(auto& col, auto& row);
void erase_line(SHORT y);
void hide_cursor();
int  get_cursor_row();
void gotoxy(SHORT x, SHORT y);

void then();
bool is_wide(wchar_t c);
int  disp_width(wstring s);
void put_line(SHORT x, SHORT y, WORD attr, wstring s);

int main() {
    //初始化//
    SetConsoleOutputCP(65001);//UTF-8，保证中文不乱码
    SetConsoleCP(65001);
    //自由改窗口//
    system("cls");
    get_console_size(C_col, C_row);
    printf("  现在你可以随便改控制台大小\n");
    then();
    //固定窗口//
    lock_window();
    get_console_size(C_col, C_row);
    set_console_size(C_col, C_row);
    int W=C_col-1;
    int H=C_row-1;//最大坐标
    H=0;//题目让我这么干的
    //设字符串//
    system("cls");
    printf("  请输入要跑的字符串(不要太长)\n");
    //先正常输入转换得字符数, 再存入s, 同时防止空串下标越界//
    string  u;//按字节读入的 UTF-8
    wstring s;//宽字符串, 支持中文
    getline(cin,u);
    int n=MultiByteToWideChar(CP_UTF8, 0, u.c_str(), u.size(), NULL, 0);
    s.resize(n);
    if(n>0) MultiByteToWideChar(CP_UTF8, 0, u.c_str(), u.size(), &s[0], n);
    //别输太长//
    int w=disp_width(s);
    if(W<=w) { printf("你输的比你窗口还大, 还想不想活了?\n"); return 0; }
    //小提示//
    system("cls");
    printf("  当程序运行时, 你可以按'ESC'退出\n");
    then();
    hide_cursor();
    system("cls");
    //运动//
    int o=0, x=0, y=0, key;//状态, 开头位置, 按键
    for(;;)
    {
        //右//
             if(x+w> W && o==0) o=1;
        else if(o==0) ++x;
        //下//
        else if(  y>=H && o==1) o=2;
        else if(o==1) ++y;
        //左//
        else if(  x<=0 && o==2) o=3;
        else if(o==2) --x;
        //上//
        else if(  y<=0 && o==3) o=0;
        else if(o==3) --y;
        //光标置首+前置空格+字符串+清除末尾与上下//
        put_line(0, y-1, 7, wstring(C_col, ' '));
        put_line(0, y, 7, wstring(x, ' ') +s +L"   ");
        put_line(0, y+1, 7, wstring(C_col, ' '));
        if(_kbhit()) {
            key=_getch();
            if(key==27) return 0;
        }
        Sleep(sleeptime);
    }
    return 0;
}//绕控制台跑的字符串

//小插件//

void then() {//-----------------------------------------按任意键继续
    printf("按任意键继续...");
    _getch();
    erase_line(get_cursor_row()); gotoxy(0, get_cursor_row());
}

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
    WriteConsoleOutputW(H_out, Buf.data(), size, Buf_Org, &region);
}

//控制台//

void lock_window() {//----------------------------------不让动窗口大小
    HWND     C_W   = GetConsoleWindow();
    LONG_PTR W_LP  = GetWindowLongPtrA(C_W, GWL_STYLE);
    SetWindowLongPtrA(C_W, GWL_STYLE, W_LP & ~WS_THICKFRAME);
}

void set_console_size(SHORT W, SHORT H) {//-------------设窗口和缓冲区大小
    //规范W,H//
    if(W<1) W=1;
    if(H<1) H=1;
    COORD C_W_max     = GetLargestConsoleWindowSize(H_out);
    if(W> C_W_max.X) W=C_W_max.X;
    if(H> C_W_max.Y) H=C_W_max.Y;
    //设窗口大小//
    SMALL_RECT W_size = { 0, 0, SHORT(W-1), SHORT(H-1) };
    SetConsoleWindowInfo(H_out, TRUE, &W_size);
    //设缓冲区大小//
    GetConsoleScreenBufferInfo(H_out, &Buf_info);
    COORD Buf_size    = { Buf_info.dwSize.X <W ? W : Buf_info.dwSize.X,
                          Buf_info.dwSize.Y <H ? H : Buf_info.dwSize.Y };
    SetConsoleScreenBufferSize(H_out, Buf_size);//放大缓冲区
    SetConsoleScreenBufferSize(H_out, {W,H}   );//缩缓冲区到没有滚动条
}

void get_console_size(auto& col, auto& row) {//---------获取控制台宽高
    GetConsoleScreenBufferInfo(H_out, &Buf_info);
    col = Buf_info.srWindow.Right  - Buf_info.srWindow.Left +1;
    row = Buf_info.srWindow.Bottom - Buf_info.srWindow.Top  +1;
}

void erase_line(SHORT y) {//----------------------------擦除指定行
    COORD pos={0,y}; DWORD x;
    FillConsoleOutputCharacter(H_out, ' ', C_col, pos, &x);
}

void hide_cursor() {//----------------------------------隐藏光标
    CONSOLE_CURSOR_INFO Cur_info;
    Cur_info.bVisible =FALSE;
    Cur_info.dwSize   =1;
    SetConsoleCursorInfo(H_out, &Cur_info);
}

int get_cursor_row() {//--------------------------------获取光标行
    GetConsoleScreenBufferInfo(H_out, &Buf_info);
    return Buf_info.dwCursorPosition.Y;
}

void gotoxy(SHORT x, SHORT y) {//-----------------------移动光标
    COORD Cur_pos={x,y};
    SetConsoleCursorPosition(H_out, Cur_pos);
}
