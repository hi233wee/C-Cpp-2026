#include "is_prime.hpp"

#include <cstdio>
#include <windows.h>
#include <conio.h>
using namespace std;
typedef unsigned long long uLL;

HANDLE H_out = GetStdHandle(STD_OUTPUT_HANDLE); //控制台输出句柄
CONSOLE_SCREEN_BUFFER_INFO Buf_info; //-----------缓冲区信息
int C_row, C_col; //------------------------------控制台大小

void get_console_size (auto& col, auto& row);
void erase_line (SHORT y);
int  get_cursor_row ();
void gotoxy (SHORT x, SHORT y);

uLL rd ();

int main () {
    // 初始化 //
    SetConsoleOutputCP(65001); // UTF-8, 保证中文不乱码
    SetConsoleCP(65001);
    get_console_size(C_col, C_row);
    system("cls");
    // 循环 //
    uLL n; char c;
    for (;;) {
        // 输入 //
        printf("请输入一个数并让我判断它是不是质数\n");
        n=rd();
        erase_line(get_cursor_row()-1);
        erase_line(get_cursor_row()-2);
        gotoxy( 0, get_cursor_row()-2);
        // 判断 //
        if (is_prime(n)) { printf("  %llu 是质数  \n", n); }
        else             { printf("  %llu 不是质数\n", n); }
        // 请求 //
        printf("请按'ENTER'继续或'ESC'退出");
        for (;;) {
            c=_getch();
            if (c==27) { erase_line(get_cursor_row()); return 0; }
            if (c==13) { break; }
        }
        erase_line(get_cursor_row());
        gotoxy( 0, get_cursor_row());
    }
    return 0;
}

// 小插件 //

uLL rd () { //------------------------------------------读数字
    int c =getchar(); uLL x=0;
    while (c!=EOF && !isdigit(c)) { c =getchar(); }
    if (c==EOF) { return 0; } // ai说有神秘原因会输入这个
    while (isdigit(c))
    { x = x*10 +c -48; c=getchar(); }
    return x;
}

// 控制台 //

void get_console_size (auto& col, auto& row) { //-------获取控制台宽高
    GetConsoleScreenBufferInfo(H_out, &Buf_info);
    col = Buf_info.srWindow.Right  - Buf_info.srWindow.Left +1;
    row = Buf_info.srWindow.Bottom - Buf_info.srWindow.Top  +1;
}

void erase_line (SHORT y) { //--------------------------擦除指定行
    COORD pos={0, y}; DWORD x;
    FillConsoleOutputCharacter(H_out, ' ', C_col, pos, &x);
}

int get_cursor_row () { //------------------------------获取光标行
    GetConsoleScreenBufferInfo(H_out, &Buf_info);
    return Buf_info.dwCursorPosition.Y;
}

void gotoxy (SHORT x, SHORT y) { //---------------------移动光标
    COORD Cur_pos={x, y};
    SetConsoleCursorPosition(H_out, Cur_pos);
}
