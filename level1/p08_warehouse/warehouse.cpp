#include <iostream>
#include <algorithm>
#include <random>
#include <conio.h>
#include <windows.h>
#include <fstream>
#include <filesystem>
using namespace std;
typedef long long LL;

random_device RD;
mt19937 RNG(RD());
inline LL Rand (LL l, LL r);

HANDLE H_out = GetStdHandle(STD_OUTPUT_HANDLE); //控制台输出句柄
CONSOLE_SCREEN_BUFFER_INFO Buf_info; //-----------缓冲区信息
SHORT C_row, C_col; //----------------------------控制台大小
const int min_W=49; //----------------------------最小宽度(给人留点发挥空间)
const int min_H=25; //----------------------------最小高度(check界面23)
void set_console_font ();
void lock_window ();
void set_console_size (SHORT W, SHORT H);
void get_console_size (auto& col, auto& row);
void hide_cursor ();
void show_cursor ();
void gotoxy (SHORT x, SHORT y);
int  get_cursor_row ();
void erase_line (SHORT y);
void set_color (int BG, int FG);

const char* COLOR_NAMES[16]= //--颜色模版
{"黑色","深蓝","深绿","青色","深红","紫色","暗黄","浅灰","深灰","亮蓝","亮绿","亮青","亮红","亮紫","黄色","白色"};
// 0      1      2      3     4      5      6      7     8      9     10     11     12     13    14     15  //

void then ();
bool is_wide (wchar_t c);
int  disp_width (wstring s);
void put_line (SHORT x, SHORT y, WORD A, wstring s);

const string STOCK=R"(stock.warehouse)"; //仓库文件
struct things
{
    string name;
    string lower; //小写
    LL n;
}; //----------------------------物品
vector<things> stock, tmp; //----库存, 临时
string tolower (string s);
void search (string name);
void update (string name, LL n);

bool load ();
bool save ();

void main_screen ();
void check_screen ();
void search_screen (string&name);
bool input_screen (string&name, LL&n);
bool confirm_screen (const string&name, const LL&n);
void in_screen ();
void out_screen ();
void reset_screen ();

int main () {
    //初始设置//
    SetConsoleOutputCP(65001); //UTF-8, 保证中文不乱码
    SetConsoleCP(65001);
    set_console_font();
    lock_window();
    set_console_size(0, 0);
    get_console_size(C_col, C_row);
    hide_cursor();
    system("cls");

    //读仓库//
    if(!load())
    {
        set_color( 0,11);
        printf("\n\t我很不幸地告诉你,\n");               then();
        printf("\t仓库打开失败了. \n");                  then();
        printf("\t不知道是什么杀毒软件把钥匙夺走了, \n"); then();
        printf("\t还是你没给我钥匙. \n");                then();
        printf("\t建议你把我加入信任名单. \n");          then();
        printf("\t不过如果你连仓库都没有, \n");          then();
        printf("\t别来找我. \n");
        set_color( 0, 7);
        return 0;
    }

    main_screen();

    //写仓库//
    if(!save())
    {
        set_color( 0,11);
        printf("\n\t要是你看见这句话了, 说明你很幸运, \n");   then();
        printf("\t因为你保存失败了. \n");                    then();
        printf("\t你之前的操作全白费了. \n");                then();
        printf("\t这就像你的文档没有保存, 突然就停电了. \n"); then();
        printf("\t当然如果你不是台式机就没有事. \n");         then();
        printf("\t至于为什么不是每次操作完就保存, \n");       then();
        printf("\t你问老师去. \n");
        set_color( 0, 7);
    }

    set_color( 0, 7);
    return 0;
} //仓库进销存

//界面//

void main_screen () { //--------------------------------主界面
    for(;;)
    {
        BACK: //标签, 又让我学到了
        tmp=stock; //为check准备
        system("cls");
        put_line(11, 7, 7, L"╔════════════════════════╗");
        put_line(11, 8, 7, L"║     ┌────────────┐     ║");
        put_line(11, 9, 7, L"║     │ 进销存菜单 │     ║");
        put_line(11,10, 7, L"║     └────────────┘     ║");
        put_line(11,11, 7, L"║        1. 查看         ║");
        put_line(11,12, 7, L"║        2. 入库         ║");
        put_line(11,13, 7, L"║        3. 出库         ║");
        put_line(11,14, 7, L"║        4. 重置         ║");
        put_line(11,15, 7, L"╟──────────┬──┬──────────╢");
        put_line(11,16, 7, L"║ 123 选择 │77│ esc 退出 ║");
        put_line(11,17, 7, L"╚══════════╧══╧══════════╝");
        put_line(17, 8,10,       L"┌────────────┐");
        put_line(17, 9,10,       L"│ 进销存菜单 │");
        put_line(17,10,10,       L"└────────────┘");
        put_line(20,11,14,          L"1. ");
        put_line(20,12,14,          L"2. ");
        put_line(20,13,14,          L"3. ");
        put_line(23,11,11,             L"查看");
        put_line(23,12,11,             L"入库");
        put_line(23,13,11,             L"出库");
        put_line(20,14, 0,          L"4. 重置"); //看不见我!
        put_line(13,16,10,   L"123 选择");
        put_line(27,16,10,                 L"esc 退出");

        int key;
        for(;;)
        {
            key=_getch();
            switch(key)
            {
                case 49: check_screen(); goto BACK; //'1'
                case 50:    in_screen(); goto BACK; //'2'
                case 51:   out_screen(); goto BACK; //'3'
                case 52: reset_screen(); goto BACK; //'4'
                case 27 :                return;
            }
        }
    }
}

void check_screen () { //-------------------------------查看
    string name=""; //搜索名
    BACKBACK:
    system("cls");
    put_line( 7, 1, 7, L"╔══════════════════════════════════╗");
    put_line( 7, 2, 7, L"║           ┌──────────┐           ║");
    put_line( 7, 3, 7, L"║           │ 你的仓库 │           ║");
    put_line( 7, 4, 7, L"║           └──────────┘           ║");
    put_line( 7, 5, 7, L"║                                  ║");
    put_line( 7, 6, 7, L"║  数量                            ║");
    put_line( 7, 7, 7, L"║                                  ║");
    put_line( 7, 8, 7, L"║  数量                            ║");
    put_line( 7, 9, 7, L"║                                  ║");
    put_line( 7,10, 7, L"║  数量                            ║");
    put_line( 7,11, 7, L"║                                  ║");
    put_line( 7,12, 7, L"║  数量                            ║");
    put_line( 7,13, 7, L"║                                  ║");
    put_line( 7,14, 7, L"║  数量                            ║");
    put_line( 7,15, 7, L"║                                  ║");
    put_line( 7,16, 7, L"║  数量                            ║");
    put_line( 7,17, 7, L"║                                  ║");
    put_line( 7,18, 7, L"║  数量                            ║");
    put_line( 7,19, 7, L"║                                  ║"); //15行
    put_line( 7,20, 7, L"║ ←↑上一页   第 7777 页   下一页↓→ ║");
    put_line( 7,21, 7, L"╟──────────┬────────────┬──────────╢");
    put_line( 7,22, 7, L"║ 方向换页 │ s 进入搜索 │ esc 返回 ║");
    put_line( 7,23, 7, L"╚══════════╧════════════╧══════════╝");
    put_line(19, 2,10,            L"┌──────────┐");
    put_line(19, 3,10,            L"│ 你的仓库 │");
    put_line(19, 4,10,            L"└──────────┘");
    put_line(10, 6, 8,    L"数量");
    put_line(10, 8, 8,    L"数量");
    put_line(10,10, 8,    L"数量");
    put_line(10,12, 8,    L"数量");
    put_line(10,14, 8,    L"数量");
    put_line(10,16, 8,    L"数量");
    put_line(10,18, 8,    L"数量");
    put_line( 9,22,10,  L"方向换页");
    put_line(20,22,10,             L"s 进入搜索");
    put_line(33,22,10,                          L"esc 返回");

    int key; size_t page=1, id, i; //无符号有个大坑
    BACK:
    //打印搜索//
    if(!name.empty()) {
        gotoxy( 9,19); set_color( 0, 8);
        fputs(("搜索 "+name).c_str(), stdout);
    }
    //页码//
    if(page==0) page =(tmp.size() +6) /7; //越下界
    if(tmp.size() < page*7 -6) page =1;   //越上界
    gotoxy(23,20); set_color( 0, 7);
    printf("%4zu", page);
    //打印物品//
    id =min(page*7, tmp.size()); //上界+1
    if(page*7>=id) //满
        for(i=page*7-7; i<id; ++i)
        {
            erase_line(i%7*2 +5); //万一你输了很长的呢
            put_line( 7, i%7*2+5, 7, L"║                                  ║");
            gotoxy(10, i%7*2 +5); set_color( 0,14);
            fputs(tmp[i].name.c_str(), stdout);
            gotoxy(15, i%7*2 +6); set_color( 0,11);
            printf("%-27lld", tmp[i].n);
        }
    if(id<page*7) //没满
        for(i=id; i<page*7; ++i)
        {
            erase_line(i%7*2 +5); //万一你输了很长的呢
            put_line( 7, i%7*2+5, 7, L"║                                  ║");
            put_line( 7, i%7*2+6, 7, L"║                                  ║");//把数量也去掉
        }
    //按键//
    for(;;)
    {
        key=_getch();
        switch(key)
        {
            case 27:           return;
            case 115: case 83: //'s' 'S'
                search_screen(name);
                page=1;
                goto BACKBACK;
            case 0: case 224:
                key=_getch();
                switch(key){
                    case 72: case 75: page--; break;
                    case 77: case 80: page++; break;
                }goto BACK;
        }
    }
}

void search_screen (string&name) { //-------------------搜索(不区分大小写)
    system("cls");
    put_line( 8, 8, 7, L"┌──────────────┬──────────────┐");
    put_line( 8, 9, 7, L"│  enter 确认  │ 输入空白返回 │");
    put_line( 8,10, 7, L"└──────────────┴──────────────┘");
    put_line(15,11, 7,        L"┌────────────────┐");
    put_line(15,12, 7,        L"│ 你的名字是啥呢 │");
    put_line(15,13, 7,        L"└────────────────┘");
    put_line(11, 9,10,    L"enter 确认");
    put_line(25, 9,10,                  L"输入空白返回"); //懒得写清除搜索了
    put_line(17,12,11,          L"你的名字是啥呢");
    //输入//
    show_cursor();
    gotoxy( 3,14); set_color( 0, 7); //改输入时的位置颜色
    getline(cin, name);
    hide_cursor();
    search(name);
}

bool input_screen (string&name, LL&n) { //--------------进出库输入
    BACK:
    system("cls");
    put_line( 8, 8, 7, L"┌──────────────┬──────────────┐");
    put_line( 8, 9, 7, L"│  enter 确认  │ 输入空白返回 │");
    put_line( 8,10, 7, L"└──────────────┴──────────────┘");
    put_line( 8,11, 7, L"┌──────┐");
    put_line( 8,12, 7, L"│      │");
    put_line( 8,13, 7, L"└──────┘");
    put_line(11, 9,10,    L"enter 确认");
    put_line(25, 9,10,                  L"输入空白返回"); //你知道写输入中返回有多难吗
    put_line(10,12,11,   L"数量");
    //输入数量//
    show_cursor();
    gotoxy( 3,14); set_color( 0, 7); //改输入时的位置颜色
    getline(cin, name); //不想再开一个
    if(name.empty()) { hide_cursor(); return 0; }
    n=atoi(name.c_str());
    if(n<=0) { //非正数
        put_line( 8,15,12, L"不是, 你输的啥? 重来!");
        gotoxy( 0,16); set_color( 0, 7);
        then();
        goto BACK;
    }
    erase_line(14);

    put_line(10,12,11,   L"名字");
    put_line( 6,15,14, L"注: 别输太长, 后果自负; 区分大小写;");
    put_line(10,16,14,     L"我会把你的空改为'_'");
    //输入名字//
    gotoxy( 3,14); set_color( 0, 7); //同上
    getline(cin, name);
    if(name.empty()) { hide_cursor(); goto BACK; }
    for(size_t i=name.size(); i--;) //改成'_'
        if(isspace(name[i])) name[i]='_'; //所有cin会停的都改

    hide_cursor();
    return 1;
}

bool confirm_screen (const string&name, const LL&n) { //确认进出库
    system("cls");
    put_line(11, 7, 7, L"╔═════════════════════════╗");
    put_line(11, 8, 7, L"║ 你的名字:               ║");
    put_line(11, 9, 7, L"║                         ║"); //太长就冲出去了
    put_line(11,10, 7, L"║ 你的数量:               ║");
    put_line(11,11, 7, L"║                         ║");
    put_line(11,12, 7, L"║ 已有数量:               ║");
    put_line(11,13, 7, L"║                         ║");
    put_line(11,14, 7, L"╟────────────┬────────────╢");
    put_line(11,15, 7, L"║ enter 确认 │  esc 返回  ║");
    put_line(11,16, 7, L"╚════════════╧════════════╝");
    put_line(13, 8,11,   L"你的名字: ");
    put_line(13,10,11,   L"你的数量: ");
    put_line(13,12,11,   L"已有数量: ");
    put_line(13,15,10,   L"enter 确认");
    put_line(27,15,10,                 L"esc 返回");

    vector<things>::iterator it=find_if(stock.begin(), stock.end(), [&name](const things&a){return a.name==name; });
    gotoxy(15, 9); set_color( 0,14); fputs(name.c_str(), stdout);
    gotoxy(15,11); set_color( 0, 3); printf("%lld", n);
    gotoxy(15,13); set_color( 0, 3); printf("%lld", it==stock.end()? 0 : it->n);

    FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE)); //清空输入缓冲'\n'
    int key;
    for(;;)
    {
        key=_getch();
        if(key==27) return 0;
        if(key==13) break;
    }
    return 1;
}

void in_screen () { //----------------------------------入库
    system("cls");
    put_line( 6, 8,10, L"=====================================");
    put_line( 6, 9,10, L"                入 库                ");
    put_line( 6,10,10, L"=====================================");
    set_color( 0, 7); gotoxy( 6,11);
    then();
    string name; LL n;
    for(;;){
        if(! input_screen(name, n)) return; //空输入退出
        if(confirm_screen(name, n)) break ; //确认了继续
    }

    update(name, n);

    system("cls");
    put_line( 6, 8,10, L"=====================================");
    put_line( 6, 9,10, L"             入 库 成 功             ");
    put_line( 6,10,10, L"=====================================");
    set_color( 0, 7); gotoxy( 6,11);
    then();
}

void out_screen () { //---------------------------------出库
    system("cls");
    put_line( 6, 8,10, L"=====================================");
    put_line( 6, 9,10, L"                出 库                ");
    put_line( 6,10,10, L"=====================================");
    put_line( 6,11,12, L"若你出库的数量大于库存, 将自动全部出库");
    set_color( 0, 7); gotoxy( 6,12);
    then();
    string name; LL n;
    for(;;){
        if(! input_screen(name, n)) return; //空输入退出
        if(confirm_screen(name, n)) break ; //确认了继续
    }
    //判断仓库有没有//
    vector<things>::iterator it=find_if(stock.begin(), stock.end(), [&name](const things&a){ return a.name==name; });
    if(it==stock.end()) {
        put_line(15,17,12, L"东西都没有你出个蛋"); //接着确认界面
        gotoxy( 0,18); set_color( 0, 7);
        then();
        return;
    }

    update(name,-n);

    system("cls");
    put_line( 6, 8,10, L"=====================================");
    put_line( 6, 9,10, L"             出 库 成 功             ");
    put_line( 6,10,10, L"=====================================");
    set_color( 0, 7); gotoxy( 6,11);
    then();
}

void reset_screen () { //-------------------------------重置
    system("cls");
    put_line( 2, 1, 7, L"你按了4, 对吧? ");
    put_line(10, 1,12,       L"4");
    put_line( 2, 2, 7, L"接下来会进行一个很危险的操作, ");
    put_line( 2, 3, 7, L"那就是会把你的仓库重置, ");
    put_line(22, 3,12,                   L"重置");
    put_line( 2, 4, 7, L"并且无法挽回. ");
    put_line( 8, 4,12,     L"无法挽回");
    put_line( 2, 5, 7, L"你考虑清楚. ");
    put_line( 2, 7,14, L"按 esc 返回, 按 enter 确认进行. ");
    put_line( 7, 7,12,    L"esc");
    put_line(20, 7,12,                 L"enter");

    int key;
    for(;;)
    {
        key=_getch();
        if(key==13) break; //enter
        if(key==27) return; //esc
    }

    erase_line(7);
    if(CopyFileA((STOCK+".copy").c_str(), STOCK.c_str(), false)) {
        put_line( 0, 7,11, L"  OK啊, 也是完成了. ");
    } else {
        put_line( 0, 7,11, L"  我去, 居然失败了. ");
    }
    load();

    gotoxy( 0, 8); set_color( 0, 7);
    then();
}

//存取//

bool load () { //---------------------------------------读取库存
    ifstream S(STOCK);
    if(!S.is_open()) return 0;
    stock.clear(); //不清等死
    things A;
    while(S >>A.name >>A.n) {
        A.lower=tolower(A.name);
        stock.push_back(A);
    }
    sort(stock.begin(), stock.end(), [](const things&a, const things&b){ return a.lower<b.lower; });
    return 1;
}

bool save () { //---------------------------------------保存
    ofstream S(STOCK+".tmp");
    if(!S.is_open()) return 0;
    for(auto i:stock)
        S <<i.name <<' ' <<i.n <<'\n';
    S.close(); //防止阻碍合并
    error_code ec;
    filesystem::rename(STOCK+".tmp", STOCK, ec); //合并
    if(ec) return 0; //有错误信息
    return 1;
}

//数据//

string tolower (string s) { //--------------------------转小写
    for(size_t i=s.size(); i--;)
        if(65<=s[i] && s[i]<=90) s[i]+=32;//用ASCLL算
    return s;
}

void search (string name) { //--------------------------将搜索到的放入tmp
    tmp.clear(); //不清等死
    name=tolower(name);
    for(size_t i=0; i<stock.size(); ++i)
        if(stock[i].lower.find(name)!=string::npos)
            tmp.push_back(stock[i]);
}

void update (string name, LL n) { //--------------------更新库存数据(区分大小写)
    vector<things>::iterator it=find_if(stock.begin(), stock.end(), [&name](const things&a){ return a.name==name; });
    //[&]所有外部引用捕获,[=]值捕获,[&x][=x]只捕获x,[=,&x]默认 值捕获,x引用//本身固定传入const &
    if(it==stock.end()) {
        if(n<0) return;
        things a;
        a.name =name;
        a.n    =n;
        a.lower=tolower(name);
        stock.push_back(a);
        sort(stock.begin(), stock.end(), [](const things&a, const things&b){ return a.lower<b.lower; });
    } else {
        it->n +=n;
        if(it->n <=0) stock.erase(it);
    }
}

//小插件//

void then () { //---------------------------------------等一下
    printf("按任意键继续... ");
    _getch();
    erase_line(get_cursor_row());
    gotoxy( 0, get_cursor_row());
}

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

void show_cursor () { //--------------------------------显示光标
    CONSOLE_CURSOR_INFO Cur_info;
    Cur_info.bVisible =true;
    Cur_info.dwSize   =100;
    SetConsoleCursorInfo(H_out, &Cur_info);
}

void gotoxy (SHORT x, SHORT y) { //---------------------移动光标
    COORD Cur_pos={x, y};
    SetConsoleCursorPosition(H_out, Cur_pos);
}

void get_console_size (auto& col, auto& row) { //-------获取控制台宽高
    GetConsoleScreenBufferInfo(H_out, &Buf_info);
    col = Buf_info.srWindow.Right  - Buf_info.srWindow.Left +1;
    row = Buf_info.srWindow.Bottom - Buf_info.srWindow.Top  +1;
}

int get_cursor_row () { //------------------------------获取光标行
    GetConsoleScreenBufferInfo(H_out, &Buf_info);
    return Buf_info.dwCursorPosition.Y;
}

void erase_line (SHORT y) { //--------------------------擦除指定行
    COORD pos={0, y}; DWORD x;
    FillConsoleOutputCharacter(H_out, 32, C_col, pos, &x); //' '
}

void set_color (int BG, int FG) { //--------------------设置颜色, BG背景, FG前景
    SetConsoleTextAttribute(H_out, (BG<<4)|FG);
}

//随机函数//

inline LL Rand (LL l, LL r) { //------------------------随机数
    uniform_int_distribution<LL> RAND(l, r);
    return RAND(RNG);
}

/*
学习成果
1.ifstream   ofstream
2.文件搜索
3.文件重命名
4.set存变量
5.比较size()最好用size_t
6.for里用++i更好
7.filesystem文件操作
*/
