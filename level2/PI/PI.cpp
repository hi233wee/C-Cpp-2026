#include "PI.hpp"

#include <iostream>
#include <algorithm>
#include <windows.h>
#include <conio.h>

#define Precision 100000LL // 必须是编译期常量, 且不超过 5e6

int main() {
    SetConsoleOutputCP(65001); // UTF-8, 保证中文不乱码
    SetConsoleCP(65001);
    // 开始计时 //
    LARGE_INTEGER frequency, start, end;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&start);

    std::cout << get_PI<Precision>;

    // 停止计时 //
    QueryPerformanceCounter(&end);
    std::printf("精度: %lld", Precision);
    std::printf("花费时间: %.4lf ms\n", (double)(end.QuadPart - start.QuadPart) /frequency.QuadPart *1000);
    return 0;
}