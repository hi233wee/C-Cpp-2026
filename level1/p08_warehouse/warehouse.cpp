#include<iostream>
#include<algorithm>
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
#include<typeinfo>
#include<cctype>
using namespace std;
typedef long long LL;

/* ==================== 随机函数 ====================*/

random_device RD;
mt19937 RNG(RD());
inline LL Rand(LL l,LL r)
{
    uniform_int_distribution<LL> RAND(l,r);
    return RAND(RNG);
}

/* ==================== 控制台工具 ==================== */

HANDLE handle_output=GetStdHandle(STD_OUTPUT_HANDLE);
void set_console_font()//----------------------------改字体
{
    CONSOLE_FONT_INFOEX a={sizeof(a)};
    a.dwFontSize.Y=16;
    wcscpy_s(a.FaceName,L"Lucida Console");
    SetCurrentConsoleFontEx(handle_output,FALSE,&a);
}
void lock_window()//---------------------------------不让动窗口大小
{
    HWND a=GetConsoleWindow();
    SetWindowLongPtrA(a,GWL_STYLE,GetWindowLongPtrA(a,GWL_STYLE)&~(LONG_PTR)(WS_THICKFRAME|WS_MAXIMIZEBOX));
    SetWindowPos(a,NULL,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_FRAMECHANGED);
}
const int min_W=49;//最小宽度(给人留点发挥空间)
const int min_H=25;//最小高度(check界面23)
void set_console_size(SHORT W,SHORT H)//-------------设窗口和缓冲区大小
{
    if(W<min_W) W=min_W;
    if(H<min_H) H=min_H;
    COORD mx=GetLargestConsoleWindowSize(handle_output);
    if(W>mx.X) W=mx.X;
    if(H>mx.Y) H=mx.Y;
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(handle_output,&a);
    COORD b={a.dwSize.X<W?W:a.dwSize.X,
             a.dwSize.Y<H?H:a.dwSize.Y};
    SetConsoleScreenBufferSize(handle_output,b);//放大缓冲区
    SMALL_RECT c={0,0,SHORT(W-1),SHORT(H-1)};
    SetConsoleWindowInfo(handle_output,TRUE,&c);//设窗口大小
    SetConsoleScreenBufferSize(handle_output,{W,H});//缩缓冲区到没有滚动条
}
void hide_cursor()//---------------------------------隐藏光标
{
    CONSOLE_CURSOR_INFO a;
    a.bVisible=0;
    a.dwSize=1;
    SetConsoleCursorInfo(handle_output,&a);
}
void show_cursor()//---------------------------------显示光标
{
    CONSOLE_CURSOR_INFO a;
    a.bVisible=1;
    a.dwSize=100;
    SetConsoleCursorInfo(handle_output,&a);
}
void gotoxy(short x,short y)//-----------------------移动光标
{
    COORD a={x,y};
    SetConsoleCursorPosition(handle_output,a);
}
int get_cursor_column()//----------------------------获取控制台列
{
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(handle_output,&a);
    return a.dwCursorPosition.X;
}
int get_cursor_row()//-------------------------------获取控制台行
{
    CONSOLE_SCREEN_BUFFER_INFO a;
    GetConsoleScreenBufferInfo(handle_output,&a);
    return a.dwCursorPosition.Y;
}
void erase_row(int y)//------------------------------擦除指定行
{
    CONSOLE_SCREEN_BUFFER_INFO d;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&d);
    int a=d.srWindow.Right-d.srWindow.Left+1;
    COORD b;DWORD c;
    b.X=0;b.Y=y;
    FillConsoleOutputCharacter(handle_output,' ',a,b,&c);
}
void set_color(int BG,int FG)//----------------------设置颜色；BG背景；FG前景
{
    SetConsoleTextAttribute(handle_output,(WORD)((BG<<4)|FG));
}
const char* COLOR_NAMES[16]=//-----------------------颜色模版
{"黑色","深蓝","深绿","青色","深红","紫色","暗黄","浅灰","深灰","亮蓝","亮绿","亮青","亮红","亮紫","黄色","白色"};
/* 0      1      2      3     4      5      6      7     8      9     10     11     12     13    14     15  */

/* ==================== 游戏数据 ==================== */

const string STOCK=R"(stock.warehouse)";//---仓库文件
struct things
{
    string name;
    string lower;//小写
    LL n;
};//------------------------------物品
vector<things> stock,tmp;//-------库存、临时

string tolower(string s)//---------------------------转小写
{
    for(size_t i=s.size();i--;)
        if(65<=s[i]&&s[i]<=90) s[i]+=32;
    return s;
}
void search(string name)//---------------------------将搜索到的放入tmp
{
    tmp.clear();//不清等死
    name=tolower(name);
    for(size_t i=0;i<stock.size();++i)
        if(stock[i].lower.find(name)!=string::npos)
            tmp.push_back(stock[i]);
}
void update(string name,LL n)//----------------------更新库存数据(区分大小写)
{
    vector<things>::iterator it=find_if(stock.begin(),stock.end(),[&name](const things&a){return a.name==name;});
    //[&]所有外部引用捕获,[=]值捕获,[&x][=x]只捕获x,[=,&x]默认 值捕获,x引用//本身固定传入const &
    if(it==stock.end())
    {
        things a;
        a.name =name;
        a.n    =n;
        a.lower=tolower(name);
        stock.push_back(a);
        sort(stock.begin(),stock.end(),[](const things&a,const things&b){return a.lower<b.lower;});
    }
    else
    {
        it->n +=n;
        if(it->n<=0) stock.erase(it);
    }
}

/* ==================== 数据存取 ==================== */

bool load()//----------------------------------------读取库存
{
    ifstream S(STOCK);
    if(!S.is_open()) return 0;
    stock.clear();//不清等死
    things A;
    while(S>>A.name>>A.n)
    {
        A.lower=tolower(A.name);
        stock.push_back(A);
    }
    sort(stock.begin(),stock.end(),[](const things&a,const things&b){return a.lower<b.lower;});
    return 1;
}
bool save()//----------------------------------------保存
{
    ofstream S(STOCK+".tmp");
    if(!S.is_open()) return 0;
    for(auto i:stock)
        S<<i.name<<' '<<i.n<<'\n';
    S.close();//防止阻碍合并
    error_code ec;
    filesystem::rename(STOCK+".tmp",STOCK,ec);//合并
    if(ec) return 0;//有错误信息
    return 1;
}

/* ==================== 界面 ==================== */

void then()//----------------------------------------等一下
{
    printf("按任意键继续……");
    _getch();
    erase_row(get_cursor_row());gotoxy(0,get_cursor_row());
}
void search_screem(string&name)//--------------------搜索(不区分大小写)
{
    int key;BACK:
    set_color(0, 7);system("cls");
                    printf("\n┌──────────────┬──────────────┐\n│  ");
    set_color(0,10);printf("enter 确认");
    set_color(0, 7);printf("  │ ");
    set_color(0,10);printf("输入空白返回");//懒得写清除搜索了
    set_color(0, 7);printf(" │\n└──────────────┴──────────────┘\n");
                    printf("       ┌────────────────┐\n");
                    printf("       │                │\n");
                    printf("       └────────────────┘\n\n");
    gotoxy(9,5); set_color(0,11);printf("你的名字是啥呢");
    show_cursor();
    gotoxy(0,7); getline(cin,name);
    hide_cursor();
    search(name);
}//写到这我突然发现，我可以把整个表打出来再打字
void check_screem()//--------------------------------查看
{
    string name="";//搜索名
    BACKBACK:
    set_color(0, 7);system("cls");
                    printf("\n╔══════════════════════════════════╗\n");
                    printf("║           ");
    set_color(0,10);printf("┌──────────┐");
    set_color(0, 7);printf("           ║\n║           ");
    set_color(0,10);printf("│ 你的仓库 │");
    set_color(0, 7);printf("           ║\n║           ");
    set_color(0,10);printf("└──────────┘");
    set_color(0, 7);printf("           ║\n");
    for(int i=15;i;--i)
                    printf("║                                  ║\n");
                    printf("║ ←↑上一页   第      页   下一页↓→ ║\n");
                    printf("╟──────────┬────────────┬──────────╢\n║ ");
    set_color(0,10);printf("方向换页");
    set_color(0, 7);printf(" │ ");
    set_color(0,10);printf("s 进入搜索");
    set_color(0, 7);printf(" │ ");
    set_color(0,10);printf("esc 返回");
    set_color(0, 7);printf(" ║\n╚══════════╧════════════╧══════════╝");

    int key;size_t page=1,id,i;//无符号有个大坑
    BACK:
    if(!name.empty()){
        gotoxy(3,19);
        set_color(0,8);fputs(("搜索 "+name).c_str(),stdout);
    }
    if(page==0) page=(tmp.size()+6)/7;//越下界
    if(tmp.size()<page*7-6) page=1;   //越上界
    id=min(page*7,tmp.size());//上界+1
    gotoxy(16,20); set_color(0, 6);printf("%4d",page);
    if(page*7>=id)//满
        for(i=page*7-7;i<id;++i)
        {
            erase_row(i%7*2+5);//万一你输了很长的呢
            gotoxy(0,i%7*2+5);
            set_color(0, 7);printf("║                                  ║");
            gotoxy(3,i%7*2+5);
            set_color(0,14);fputs(tmp[i].name.c_str(),stdout);
            gotoxy(3,i%7*2+6);
            set_color(0, 1);printf("数量 ");
            set_color(0, 3);printf("%-27lld",tmp[i].n);
        }
    if(id<page*7)//没满
        for(i=id;i<page*7;++i)
        {
            erase_row(i%7*2+5);erase_row(i%7*2+6);//万一你输了很长的呢
            gotoxy(0,i%7*2+5);
            set_color(0, 7);printf("║                                  ║\n");
                            printf("║                                  ║\n");
        }
    for(;;)
    {
        key=_getch();
        switch(key)
        {
            case 27:           return;
            case 's':case 'S':
                search_screem(name);
                page=1;
                goto BACKBACK;
            case 0:case 224:
                key=_getch();
                switch(key){
                    case 72:case 75: page--;break;
                    case 77:case 80: page++;break;
                }goto BACK;
        }
    }
}//ai建议我把每页显示条目数放在一个常量里，方便改，可我就要7
bool input_screem(string&name,LL&n)
{
    int key;BACK:
    set_color(0, 7);system("cls");
                    printf("\n┌──────────────┬──────────────┐\n│  ");
    set_color(0,10);printf("enter 确认");
    set_color(0, 7);printf("  │ ");
    set_color(0,10);printf("输入空白返回");//你知道写输入中返回有多难吗
    set_color(0, 7);printf(" │\n└──────────────┴──────────────┘\n");
                    printf("┌──────┐\n│      │ ");
    set_color(0,14);printf("注：别输太长，后果自负；区分大小写\n");
    set_color(0, 7);printf("└──────┘\n\n");

    show_cursor();
    gotoxy(2,5); set_color(0,11);printf("数量");
    gotoxy(0,7); getline(cin,name);//不想再开一个
    if(name.empty()){
        hide_cursor();
        return 0;}
    n=atoi(name.c_str());
    if(n<=0){//非正数
        set_color(0,12);printf("不是，你输的啥？重来！");
        goto BACK;
    }
    erase_row(7);
    gotoxy(2,5); set_color(0,11);printf("名字");
    gotoxy(0,7); getline(cin,name);
    if(name.empty()){
        hide_cursor();
        goto BACK;}
    hide_cursor();
    return 1;
}
bool confirm_screem(const string&name,const LL&n)
{
    FlushConsoleInputBuffer(handle_output);//清空输入缓冲'\n'
    set_color(0, 7);system("cls");
                    printf("\n╔═════════════════════════╗\n║ ");
    set_color(0,11);printf("你的名字：");
    set_color(0, 7);printf("              ║\n");
                    printf("║                         ║\n");//太长就冲出去了
    set_color(0, 7);printf("║ ");
    set_color(0,11);printf("你的数量：");
    set_color(0, 7);printf("              ║\n║                         ║\n║ ");
    set_color(0,11);printf("已有数量：");
    set_color(0, 7);printf("              ║\n║                         ║\n");
                    printf("╟────────────┬────────────╢\n║ ");
    set_color(0,10);printf("enter 确认");
    set_color(0, 7);printf(" │  ");
    set_color(0,10);printf("esc 返回");
    set_color(0, 7);printf("  ║\n╚════════════╧════════════╝\n");

    vector<things>::iterator it=find_if(stock.begin(),stock.end(),[&name](const things&a){return a.name==name;});
    gotoxy(4,3); set_color(0,14);fputs(name.c_str(),stdout);
    gotoxy(4,5); set_color(0, 3);printf("%lld",n);
    gotoxy(4,7); set_color(0, 3);printf("%lld",it==stock.end()?0:it->n);

    int key;
    for(;;)
    {
        key=_getch();
        if(key==27) return 0;
        if(key==13) break;
    }
    return 1;
}
void in_screem()
{
    set_color(0, 7);system("cls");
    set_color(0,10);printf("\n=====================================\n");
                    printf("                 入 库               \n");
                    printf("=====================================\n");
    set_color(0, 7);then();
    string name;LL n;
    for(;;){
        if(! input_screem(name,n)) return;
        if(confirm_screem(name,n)) break ;
    }
    update(name,n);
    set_color(0, 7);system("cls");
    set_color(0,10);printf("\n=====================================\n");
                    printf("              入 库 成 功            \n");
                    printf("=====================================\n");
    set_color(0, 7);then();
}//其实这两个可以合起来写
void out_screem()
{
    set_color(0, 7);system("cls");
    set_color(0,10);printf("\n=====================================\n");
                    printf("                 出 库               \n");
                    printf("=====================================\n");
    set_color(0,12);printf("若你出库的数量大于库存，将自动全部出库\n");
    set_color(0, 7);then();
    string name;LL n;
    for(;;){
        if(! input_screem(name,n)) return;
        if(confirm_screem(name,n)) break ;
    }
    update(name,-n);
    set_color(0, 7);system("cls");
    set_color(0,10);printf("\n=====================================\n");
                    printf("              出 库 成 功            \n");
                    printf("=====================================\n");
    set_color(0, 7);then();
}
void reset_screem()
{
    set_color(0, 7);system("cls");
                    printf("\n  你按了");
    set_color(0,12);printf("4");
    set_color(0, 7);printf("，对吧？\n");
                    printf("  接下来会进行一个很危险的操作，\n");
                    printf("  那就是会把你的仓库");
    set_color(0,12);printf("重置");
    set_color(0, 7);printf("，\n  并且");
    set_color(0,12);printf("无法挽回");
    set_color(0, 7);printf("。\n  你考虑清楚。\n\n");
    set_color(0,14);printf("  按 ");
    set_color(0,12);printf("esc");
    set_color(0,14);printf(" 返回，按 ");
    set_color(0,12);printf("enter");
    set_color(0,14);printf(" 确认进行。\n");

    int key;
    for(;;)
    {
        key=_getch();
        if(key==13) break; //enter
        if(key==27) return;//esc
    }
    erase_row(7);
    gotoxy(0,7);
    if(CopyFileA((STOCK+".copy").c_str(),STOCK.c_str(),FALSE)){
        set_color(0,11);printf("  OK啊，也是完成了。\n");}
    else{
        set_color(0,11);printf("  我去，居然失败了。\n");}
    load();
    then();
}
void main_screem()
{
    for(;;)
    {
        BACK://标签，又让我学到了
        tmp=stock;//为check准备
        set_color(0, 7);system("cls");
                        printf("\n╔════════════════════════╗\n║     ");
        set_color(0,10);printf("┌────────────┐");
        set_color(0, 7);printf("     ║\n║     ");
        set_color(0,10);printf("│ 进销存菜单 │");
        set_color(0, 7);printf("     ║\n║     ");
        set_color(0,10);printf("└────────────┘");
        set_color(0, 7);printf("     ║\n║        ");
        set_color(0,14);printf("1. ");
        set_color(0,11);printf("查看");
        set_color(0, 7);printf("         ║\n║        ");
        set_color(0,14);printf("2. ");
        set_color(0,11);printf("入库");
        set_color(0, 7);printf("         ║\n║        ");
        set_color(0,14);printf("3. ");
        set_color(0,11);printf("出库");
        set_color(0, 7);printf("         ║\n║        ");
        set_color(0, 0);printf("4. 重置");
        set_color(0, 7);printf("         ║\n╟──────────┬──┬──────────╢\n║ ");
        set_color(0,10);printf("123 选择");
        set_color(0, 7);printf(" │77│ ");
        set_color(0,10);printf("esc 退出");
        set_color(0, 7);printf(" ║\n╚══════════╧══╧══════════╝\n");

        int key;
        for(;;)
        {
            key=_getch();
            switch(key)
            {
                case '1': check_screem();goto BACK;
                case '2':    in_screem();goto BACK;
                case '3':   out_screem();goto BACK;
                case '4': reset_screem();goto BACK;
                case 27 :                return;
            }
        }
    }
}

/* ==================== 主函数 ==================== */

int main()
{
    /* ---------- 初始设置 ---------- */
    SetConsoleOutputCP(65001);//UTF-8，保证中文不乱码
    SetConsoleCP(65001);
    set_console_font();
    lock_window();
    set_console_size(0,0);
    hide_cursor();
    system("cls");

    if(!load())
    {
        set_color(0,11);
        printf("\n\t我很不幸地告诉你,\n");                then();
        printf("\t仓库打开失败了。\n");                 then();
        printf("\t不知道是什么杀毒软件把钥匙夺走了，\n");then();
        printf("\t还是你没给我钥匙。\n");               then();
        printf("\t建议你把我加入信任名单。\n");         then();
        printf("\t不过如果你连仓库都没有，\n");         then();
        printf("\t别来找我。\n");
        set_color(0, 7);
        return 0;
    }
    main_screem();
    if(!save())
    {
        set_color(0,11);
        printf("\n\t要是你看见这句话了，说明你很幸运，\n");   then();
        printf("\t因为你保存失败了。\n");                  then();
        printf("\t你之前的操作全白费了。\n");              then();
        printf("\t这就像你的文档没有保存，突然就停电了。\n");then();
        printf("\t当然如果你不是台式机就没有事。\n");       then();
        printf("\t至于为什么不是每次操作完就保存，\n");     then();
        printf("\t你问老师去。\n");
        set_color(0, 7);
        return 0;
    }

    set_color(0,7);
    return 0;
}
/*
╔════╦════╗
║    ║    ║
╠════╬════╣
║    ║    ║
╚════╩════╝*/
/*
┌────┬────┐
│    │    │
├────┼────┤
│    │    │
└────┴────┘*/
/*
╒════╤════╕
│    │    │
╞════╪════╡
│    │    │
╘════╧════╛*/
/*
╓────╥────╖
║    ║    ║
╟────╫────╢
║    ║    ║
╙────╨────╜*/
/*
╔════════════════════════╗
║     ┌────────────┐     ║
║     │ 进销存菜单 │     ║
║     └────────────┘     ║
║        1. 查看         ║
║        2. 入库         ║
║        3. 出库         ║
║        4. 重置         ║
╟──────────┬──┬──────────╢
║ 123 选择 │77│ esc 退出 ║
╚══════════╧══╧══════════╝*/
/*
╔══════════════════════════════════╗
║           ┌──────────┐           ║
║           │ 你的仓库 │           ║
║           └──────────┘           ║
║  a                               ║
║  数量 1                          ║
║                                  ║
║  ...                             ║
║                                  ║
║                                  ║
║                                  ║
║                                  ║
║                                  ║
║                                  ║
║                                  ║
║                                  ║
║                                  ║
║                                  ║
║                                  ║
║ ←↑上一页   第 7777 页   下一页↓→ ║
╟──────────┬────────────┬──────────╢
║ 方向换页 │ s 进入搜索 │ esc 返回 ║
╚══════════╧════════════╧══════════╝*/
/*
===========================
           入 库
===========================
===========================
           出 库
===========================*/
/*
┌──────────────┬──────────────┐
│  enter 确认  │ 输入空白返回 │
└──────────────┴──────────────┘
┌──────┐┌──────┐
│ 数量 ││ 名称 │ 注：别输太长，后果自负；区分大小写
└──────┘└──────┘*/
/*
╔═════════════════════════╗
║ 你的名字：              ║
║                         ║//太长就冲出去了
║ 你的数量：              ║
║                         ║
║ 已有数量：              ║
║                         ║
╟────────────┬────────────╢
║ enter 确认 │  esc 返回  ║
╚════════════╧════════════╝*/