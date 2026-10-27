#pragma once // 头文件包含保护

#include <iostream>

typedef unsigned short int ui16;
typedef unsigned long long ui64;

ui16 log2(ui64 n) {
    ui16 res = 0;
    while (n >>= 1) { ++res; }
    return res;
}

#undef ui64
#undef ui16