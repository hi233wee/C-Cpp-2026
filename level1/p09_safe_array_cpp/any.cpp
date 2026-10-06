//#include <bits/stdc++.h> //gcc //一个头搞定
using namespace std; // 不写就全加std::
#include <iostream>

#include <random>
random_device RD;
mt19937 RNG(RD());
uniform_int_distribution<int> RAND(1, 7);
inline int Rand () {
    return RAND(RNG);
}

#include <vector>
#include <any> // c++17

vector<any> safe_array;
int main () {
    //基础展示//
    safe_array.push_back(7u); //unsigned int
    safe_array.push_back(7.7); //double
    safe_array.push_back(-7LL); //long long
    safe_array.push_back('7'); //char
    safe_array.push_back(L"7"); //const wchar_t* ("7\0")
    //展示高级货//
    short a = Rand();
    char c = Rand()+'0';
    char* b = &c;
    if (a+'0' < *b) //大概率是5 6 7
        safe_array.push_back(b); // char* //指针
    else
        safe_array.push_back(a); // short

    struct s1 { int x; }seven1 ={7};
    class s2 { public: int x; }seven2 ={7};
    switch (Rand()) {
    case 1: safe_array.push_back(7);             break;
    case 2: safe_array.push_back(7L);            break;
    case 3: safe_array.push_back(7uLL);          break;
    case 4: safe_array.push_back(string("7"));   break;
    case 5: safe_array.push_back(wstring(L"7")); break;
    case 6: safe_array.push_back(seven1);        break;
    case 7: safe_array.push_back(seven2);        break;
    }
    //有个问题, 如果是函数的引用形参不能直接存, 但可以取地址

    //取用// any_cast<>() 相当于一个变量/常量
    //有一个safe_array的共同点 直接取要判断类型
    //而且还不给用auto // 左边的auto类型一定和右边<>里一样
    auto x_1 = any_cast<unsigned int>  (safe_array[0]);
    auto x_2 = any_cast<double>        (safe_array[1]);
    auto x_3 = any_cast<long long>     (safe_array[2]);
    auto x_4 = any_cast<char>          (safe_array[3]);
    auto x_5 = any_cast<const wchar_t*>(safe_array[4]);
    cout <<x_1 <<'\n'
         <<x_2 <<'\n'
         <<x_3 <<'\n'
         <<x_4 <<'\n';
    wcout<<x_5 <<'\n';

    if (safe_array[5].has_value()) {
        if (safe_array[5].type() == typeid(short)) { //好像在any里有typeid的头
            printf("%d 1\n",  any_cast<short>(safe_array[5]));
        } else {
            printf("%c 2\n", *any_cast<char*>(safe_array[5]));
        }
    }

    const type_info* type = &safe_array[6].type(); //为什么不能直接拷贝
    if (*type == typeid(int))
        printf("%d 1\n", any_cast<int>(safe_array[6]));

    if (*type == typeid(long))
        printf("%ld 2\n", any_cast<long>(safe_array[6]));

    if (*type == typeid(unsigned long long))
        printf("%llu 3\n", any_cast<unsigned long long>(safe_array[6]));

    if (*type == typeid(const string))
        printf("%s 4\n", any_cast<const string>(safe_array[6]).c_str());

    if (*type == typeid(const wstring))
        wprintf(L"%s 5\n", any_cast<const wstring>(safe_array[6]).c_str());

    if (*type == typeid(s1))
        printf("%d 6\n", any_cast<s1>(safe_array[6]).x);

    if (*type == typeid(s2))
        printf("%d 7\n", any_cast<s2>(safe_array[6]).x);

    //展示乱取//
    try { auto p = any_cast<wstring>(safe_array[4]); }
    catch (const bad_any_cast& e) {
        printf("%s\n", e.what());
    }

    if (auto* p = any_cast<wchar_t*>(&safe_array[4])) //注意这有个&
        printf("You shouldn't see this message.\n");
    else if (p==nullptr) printf("error\n");
    return 0;
}
