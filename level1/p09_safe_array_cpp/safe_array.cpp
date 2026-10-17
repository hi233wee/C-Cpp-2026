#include "safe_array.hpp"

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <numeric>
#include <windows.h>
using namespace std;

static void title(const char* s) {
    putchar('\n');
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║  %-65s  ║\n", s);
    printf("╚══════════════════════════════════════════════════════════╝\n");
}
static void sub (const char* s) { printf("\n  ── %s ──  \n", s); }
static void note(const char* s) { printf("  %s\n",           s); }

// 打印 my_vector ( 要求 T 能 << 输出 )
template<class T>
static void show(const char* sig, const my_vector<T>& v, const char* note = "") {
    printf("  %-37s  ", sig);
    cout << v;
    printf(";   n = %zu; cap = %zu", v.size(), v.capacity());
    if (note && *note) { printf("   %s", note); }
    putchar('\n');
}

int main() {
    SetConsoleOutputCP(65001); // UTF-8, 保证中文不乱码
    SetConsoleCP(65001);
    system("cls");

    setvbuf(stdout, nullptr, _IONBF, 0);   // 关缓冲: 实时输出, 崩了也能看到位置
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║           自实现容器  my_vector<T>  /  my_any            ║\n");
    printf("║                    全 功 能 演 示                        ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");


    // my_vector //
    title("第一章  my_vector<T> —— 动态数组");

    // 1. 构造 //
    sub("1.1  构造: 默认 / 初始化列表 / 拷贝 / 移动");
    my_vector<int> v1;                  show("my_vector<int> v1", v1);
    my_vector<int> v2 = {1, 2, 3};      show("my_vector<int> v2 = {1, 2, 3}", v2);
    my_vector<int> v3 = v2;             show("my_vector<int> v3 = v2", v3);
    my_vector<int> v4 = std::move(v3);  show("my_vector<int> v4 = std::move(v3)", v4);
                                        show("v3", v3);

    // 2. 能装 //
    sub("1.2  能装: string / double / 嵌套 / 自定义类型");
    my_vector<string> sv = {"hello", "world", "!"};
    show("my_vector<string>", sv);

    my_vector<double> dv = {1.5, 2.5, 3.5};
    show("my_vector<double>", dv);

    my_vector<my_vector<int>> iv;
    iv.in(my_vector<int>{1, 2});
    iv.in(my_vector<int>{3});
    printf("  %-30s %zu 个元素, [0].size=%zu, [1][0]=%d\n",
                "my_vector<my_vector<int>>", iv.size(), iv[0].size(), iv[1][0]);

    struct Point { int x, y; };
    my_vector<Point> pv;
    pv.in(Point{1, 2});
    pv.in(Point{3, 4});
    printf("  %-30s %zu 个, 第一个 = (%d, %d)\n",
                "my_vector<struct Point>", pv.size(), pv[0].x, pv[0].y);
    
    // 3. 容量接口 //
    sub("1.3  容量接口: size / capacity / empty / reserve / resize");
                        show("v2", v2);
    v2.resize(7);       show("resize(7)", v2, "初始伸长");
    v2.resize(2);       show("resize(2)", v2, "截短");
    v2.resize(7, 7);    show("resize(7, 7)", v2, "不够的位置填 7");
    v2.reserve(10);     show("reserve(10)", v2, "只留坑不填");
    v2.reserve(0);      show("reserve(0)", v2, "收缩到实际大小");
    printf("  %-30s %d\n", "empty()", int(v2.empty()));
    printf("  %-30s %d\n", "size()", int(v2.size()));
    printf("  %-30s %d\n", "capacity()", int(v2.capacity()));

    // 4. 扩容 //
    sub("1.4  扩容：翻倍策略，均摊 O(1)");
    my_vector<int> p;
    size_t last = 0;
    printf("  ");
    for (int i = 0; i < 100; ++i) {
        if (p.capacity() != last) { last = p.capacity(); printf("cap=%zu ", last); }
        p.in(i);
    }
    putchar('\n');
    note("容量按 7 → 14 → 28 → 56 → 112 → ... 翻倍, 1000 个元素只需 9 次分配");
    
    // 5. 插入 //
    sub("1.5  插入 in() —— 9 个重载");
                            show("v1", v1);
    v1.in();                show("in()", v1, "默认值 T{}");
    int lv = 8;
    v1.in(lv);              show("in(lv)", v1, "左值 -> 拷贝");
    v1.in(9);               show("in(9)", v1, "右值 -> 移动");
    v1.in({10, 11});        show("in({10, 11})", v1, "一次追加一批");
    int ev = 0;
    v1.in(0, ev);           show("in(0, ev)", v1, "插到中间(左值)");
    v1.in(0, 99);           show("in(0, 99)", v1, "插到中间(右值)");
    v1.in(0, 2, 100);       show("in(0, 2, 100)", v1, "插 count 个");
    v1.in(0, {-1, -2});     show("in(0, {-1, -2})", v1, "位置 + 列表");
    int arr[2] = {7, 7};    printf("  arr[0] = %d, arr[1] = %d\n", arr[0], arr[1]);
    v1.in(0, arr, arr + 2); show("in(0, arr, arr + 2)", v1, "区间");

    try { (void)v1.in(77, 0); }
    catch (const my_error& e) { note( ( "v1.in(77, 0): " + string(e.what()) ).c_str() ); }
    try { (void)v1.in(-1, 0); }
    catch (const my_error& e) { note( ( "v1.in(-1, 0): " + string(e.what()) ).c_str() ); }
    note("注意：插入越界抛异常，而不是 UB, 也不是自动扩容");
    
    // 6. 删除 //
    sub("1.6  删除 out() —— 3 个重载");
                    show("v1", v1);
    v1.out();       show("out()", v1, "删末尾");
    v1.out(0);      show("out(0)", v1, "删一个");
    v1.out(1, 3);   show("out(1, 3)", v1, "删一段");
    v1.clear();     show("clear()", v1, "清空");

    try { (void)v1.out(77); }
    catch (const my_error& e) { note( ( "v1.out(77): " + string(e.what()) ).c_str() ); }
    try { (void)v1.out(-1); }
    catch (const my_error& e) { note( ( "v1.out(-1): " + string(e.what()) ).c_str() ); }
    note("注：删除越界抛异常，而不是 UB");

    // 7. 下标访问 //
    sub("1.7  operator[] —— 和 std::vector 不同，这个带检查");
                                printf("  v2[1] = %d\n", v2[1]);
    v2[1] = 99;                 show("v2[1] = 99", v2, "写入后");
    try { (void)v2[77]; }
    catch (const my_error& e) { note( ( "v2[77]: " + string(e.what()) ).c_str() ); }
    try { (void)v2[-1]; }
    catch (const my_error& e) { note( ( "v2[-1]: " + string(e.what()) ).c_str() ); }
    note("注：下标越界抛异常，而不是 UB, 也不是自动扩容");

    // 8. 迭代器 //
    sub("1.8  迭代器就是裸指针 —— 全套 STL 算法都能用");
    show("v1", v1);
    sort(v1.begin(), v1.end());                 show("std::sort", v1);
    reverse(v1.begin(), v1.end());              show("std::reverse", v1 );

    printf("  %-40s %d\n", "std::accumulate(begin, end, 0)",
        accumulate(v1.begin(), v1.end(), 0));

    auto it = find(v1.begin(), v1.end(), 7);
    printf("  %-40s %s\n", "std::find(begin, end, 7)",
        it != v1.end() ? "找到了" : "没找到");

    printf("  %-40s %zu\n", "std::max_element(begin, end) - begin",
        size_t(max_element(v1.begin(), v1.end()) - v1.begin()));

    // 9. 赋值运算符 + swap //
    sub("1.9  赋值运算符 + swap");
    my_vector<int> i1 = {1, 2}, i2 = {9, 9, 9};
                        show("i1", i1);
                        show("i2", i2);
    i1 = v1;            show("i1 = v1", i1, "拷贝赋值");
    i1 = move(i2);      show("i1 = move(i2)", i1, "移动赋值");
                        show("i2", i2, "移动后");
    swap(i1, v3);       show("swap(i1, v3)", i1, "i1和空容器交换");

    // 10. 比较 //
    sub("1.10  比较 —— == != 和 一个 <=> 生成四个");
    my_vector<int> x = {1, 2, 3}, y = {1, 2, 4}, z = {1, 2, 3};
    p.clear();
    printf("  x={1,2,3}  y={1,2,4}  z={1,2,3}  p={}\n");
    printf("  %-12s %d        %-12s %d\n", "x == z", int(x == z), "x != y", int(x != y));
    printf("  %-12s %d        %-12s %d\n", "x <  y", int(x <  y), "y >  x", int(y >  x));
    printf("  %-12s %d        %-12s %d\n", "x <= z", int(x <= z), "x >= z", int(x >= z));
    printf("  %-12s %d      (前缀更小, 字典序)\n", "p < x", int(p < x));

    
    // my_any //
    title("第二章  my_any —— 类型擦除的任意值容器");

    // 1. 构造 //
    sub("2.1  构造：空 / 装值 / 拷贝 / 移动");
    my_any e;                   printf("  %-35s has_value=%d\n", "my_any e", int(e.has_value()));
    my_any i = 42;              printf("  %-35s has_value=%d\n", "my_any i = 42  [int]", int(i.has_value()));
    my_any s = string("hello"); printf("  %-35s has_value=%d\n", "my_any s = \"hello\"  [string]", int(s.has_value()));
    my_any d = 3.14;            printf("  %-35s has_value=%d\n", "my_any d = 3.14  [double]", int(d.has_value()));
    my_any cp = i;              printf("  %-35s has_value=%d\n", "my_any cp = i  拷贝[int]", int(cp.has_value()));
    my_any mv = move(cp);       printf("  %-35s has_value=%d\n", "my_any mv = move(cp)  移动[int]", int(mv.has_value()));
                                printf("  被移走的cp  has_value=%d\n", int(cp.has_value()));
                                
    // 2. 装值 //
    sub("2.2  emplace<T>(args...) —— 原地构造，连多参构造都能用");
    struct Pair3 { int a, b, c; Pair3(int x, int y, int z) : a(x), b(y), c(z) {} };
    my_any p3;
    Pair3& pr = p3.emplace<Pair3>(1, 2, 3);
    printf("  %-45s (%d, %d, %d)\n", "p3.emplace<Pair3>(1,2,3)",
                cast<Pair3>(p3).a, cast<Pair3>(p3).b, cast<Pair3>(p3).c);
    pr.a = 40;
    printf("  %-45s (%d, %d, %d)\n", "Pair3& pr = p3.emplace; pr.a = 40; p3:",
                cast<Pair3>(p3).a, cast<Pair3>(p3).b, cast<Pair3>(p3).c);\
                
    // 3. 装值自由函数 //
    sub("2.3  make_my_any<T>(args...) —— 自由函数版的多参构造");
    my_any mp = make_my_any<Pair3>(4, 5, 6);
    printf("  %-30s (%d, %d, %d)\n", "make_my_any<Pair3>(4,5,6)",
                cast<Pair3>(mp).a, cast<Pair3>(mp).b, cast<Pair3>(mp).c);

    // 4. 类型信息 //
    sub("2.4  type() —— 运行时类型信息");
    printf("  %-30s %s\n", "i.type()  [int]", i.type().name());
    printf("  %-30s %s\n", "s.type()  [string]", s.type().name());
    printf("  %-30s %s\n", "  type()  [空]", "(会抛异常, 见 2.9)");

    // 5. 取值 //
    sub("2.5  cast<> —— 四个重载，类型安全取值");
                            printf("  %-40s %d\n", "cast<int>(i)", cast<int>(i));
                            printf("  %-40s %d\n", "cast<int>(i) = 99", cast<int>(i) = 99);
                            printf("  %-40s %s\n", "cast<int>(&i)", cast<int>(&i) ? "非空" : "nullptr");
    const my_any& ci = i;   printf("  %-40s %d\n", "const my_any& ci = i; cast<int>(ci)", cast<int>(ci));
    my_any* nul = nullptr;  printf("  %-40s %s\n", "my_any* nul = nullptr; cast<int>(nul)", cast<int>(nul) ? "非空" : "nullptr");

    // 6. 赋值运算符 + swap //
    sub("2.6  operator= —— 赋值 / 拷贝 / 移动");
    my_any a1, a2, a3;
    a2 = a1 = 123;                  printf("  %-30s %d\n", "a2 = a1 = 123", cast<int>(a1));
    a1 = string("int->string");     printf("  %-30s %s\n", "a1 = string(\"int->string\")", cast<string>(a1).c_str());
    a3 = move(a2);                  printf("  %-30s %d\n", "a3 = move(a2)", cast<int>(a3));
                                    printf("  被移走的a2  has_value=%d\n", int(a2.has_value()));
    swap(a1, a3);                   printf("  %-30s a1:%d  a3:%s\n", "swap(a1, a3)", cast<int>(a1), cast<string>(a3).c_str());
    a1.clear();
    printf("  %-30s has_value=%d\n", "a1.clear()", int(a1.has_value()));

    // 7. 异常安全 //
    sub("2.7  异常安全 —— 取错类型抛异常，本体不受影响");
    try { (void)cast<double>(i); }
    catch (const my_error& ex) { printf("  %-30s %s\n", "cast<double>(i)", string(ex.what()).c_str()); }
                                printf("  %-30s %d   (还活着)\n", "cast<int>(i)", cast<int>(i));
    try { (void)e.type(); }
    catch (const my_error& ex) { printf("  %-30s %s\n", "e.type()  (nullptr)", string(ex.what()).c_str()); }

    // 8. 隐式类型转换 //
    sub("2.8  operator T() —— 不用写 cast 也能取");
    int ii = i;         printf("  %-30s %d\n", "int ii = i", ii);
    string ss = s;      printf("  %-30s %s\n", "string ss = s", ss.c_str());

    // 9. 比较 //
    sub("2.9  比较 —— 值比较 (空值最小)");
    my_any n0 = 0, n1 = 1, n1_ = 2, ne;
    printf("  %-30s %d\n", "n0 == n1", int(n0 == n1));
    printf("  %-30s %d\n", "n1 == n1_", int(n1 == n1_));
    printf("  %-30s %d\n", "n1 < n1_", int(n1 < n1_));
    printf("  %-30s %d\n", "ne < n1", int(ne < n1));
    printf("  %-30s %d\n", "s == d", int(s == d));
    printf("  %-30s %d\n", "(s <=> d) == partial_ordering::unordered",
        int((s <=> d) == partial_ordering::unordered));

    // 10. 输出 //
    sub("2.10  输出 —— << 重载");
    printf("  %-30s", "cout << i");   cout << i << '\n';
    printf("  %-30s", "cout << s");   cout << s << '\n';

    // 组合使用 //
    title("第三章  组合使用 —— 威力在叠加");

    // 1. my_vector<my_any> //
    sub("3.1  my_vector<my_any> —— 一个容器装不同类型");
    my_vector<my_any> het;
    het.in(my_any(42));
    het.in(my_any(string("文本")));
    het.in(my_any(3.14));
    het.in(my_any(true));
    printf("  %-30s %zu 个元素:\n", "my_vector<my_any>", het.size());
    for (size_t i = 0; i < het.size(); ++i) {
        printf("      [%zu] 类型=%-12s ", i, het[i].type().name());
        if      (het[i].type() == typeid(int))      { printf("值=%d\n",    cast<int>(het[i])); }
        else if (het[i].type() == typeid(string))   { printf("值=%s\n",    cast<string>(het[i]).c_str()); }
        else if (het[i].type() == typeid(double))   { printf("值=%.2f\n",  cast<double>(het[i])); }
        else if (het[i].type() == typeid(bool))     { printf("值=%s\n",    cast<bool>(het[i]) ? "true" : "false"); }
    }

    // 2. 排序 //
    sub("3.2  std::sort 直接排 my_vector<my_any>");
    my_vector<my_any> sortable;
    sortable.in(5); sortable.in(1); sortable.in(3);
    sortable.in(2); sortable.in(4); // 构造没加 explicit, 直接用 int 转 my_any
    sort(sortable.begin(), sortable.end());         printf("  %-30s ", "after std::sort");
    for (size_t i = 0; i < sortable.size(); ++i) { printf("%d ", cast<int>(sortable[i])); }
    printf("\n");

    // 3. 嵌套 //
    sub("3.3  my_any 里装一个 my_vector —— 任意嵌套");
    my_any boxed;
    boxed.emplace<my_vector<int>>(my_vector<int>{1, 2, 3});
    show("cast<my_vector<int>>(boxed)", cast<my_vector<int>>(boxed));
    cast<my_vector<int>>(boxed).in(4);
    show("cast<my_vector<int>>(boxed)", cast<my_vector<int>>(boxed));

    // 4. 压力测试 //
    sub("3.4  压力测试: 10 万个数");
    my_vector<int> big;
    for (int i = 0; i < 100000; ++i) { big.in(i); }
    printf("  %-30s n=%zu cap=%zu\n", "big", big.size(), big.capacity());
    printf("  %-30s %lld\n", "std::accumulate(begin,end,0LL)", accumulate(big.begin(), big.end(), 0LL));
    printf("  %-30s %d\n", "std::is_sorted", int(is_sorted(big.begin(), big.end())));

    putchar('\n');
    printf("╔══════════════════════════════════════════════════════════╗\n");
    printf("║                        演 示 结 束                       ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n");
    system("pause");
    return 0;
}
// 这里展示使用