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

const string INBOUND=R"(inbound)";//-------待入库文件名
const string STOCK=R"(stock)";//-----------仓库文件名
const string EXTENTION=R"(.warehouse)";//--后缀
struct things
{
    string name;
    string lower;//小写
    LL n;
};
map<string,string> pinyin;//---------------拼音对照表
vector<things> stock,tmp;//----------------库存、临时

void search(string name)
{
    //暴力
    //修改tmp
    
}

/* ==================== 数据存取 ==================== */

bool load_pinyin()
{
    //检查文件,返回成功否
    //读取
}
bool load()//----------------------------------------读取库存
{
    //检查文件,返回成功否
    //读取
}
bool save(vector<things>& a,bool o)
{
    //临时,写入,覆盖,删除
    //返回成功否
}

/* ==================== 界面 ==================== */

void search_screem()
{
    //刚进是搜索    注意光标
    //搜索时 enter确认
    //搜索后 方向键换页、s搜索、数字选择
    //esc退出
    //返回选择的名字
    //底部引导
    string name;
    search(name);
}
void check_screem()
{
    //方向键换页、s进入搜索、数字选择
    //选择后输入数字    注意光标
    //enter确认     注意数量
    //esc返回
    //底部引导
    search_screem();
}
void in_screem()
{
    //输入          注意光标
    //enter确认
    //两次
    //esc返回
    //底部引导
}
void out_screem()
{
    //输入          注意光标
    //enter确认     注意数量
    //两次
    //esc返回
    //底部引导
}
void main_screem()
{
    //检查文件,读取
    //加载拼音表
    //数字选择
    //esc退出
    //底部引导
    //检查文件,保存
    load_pinyin();
    load();
    int key=_getch();
    for(;;)
    {
        switch(key)
        {
            case 1: check_screem();break;
            case 2:    in_screem();break;
            case 3:   out_screem();break;
        }
        if(key==27) return;
    }
}

/* ==================== 主函数 ==================== */

int main()
{
    /* ---------- 初始设置 ---------- */
    SetConsoleOutputCP(65001);//UTF-8，保证中文不乱码
    SetConsoleCP(65001);

    main_screem();

    set_color(0,7);
    return 0;
}