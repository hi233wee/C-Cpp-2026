/*
    普通长除算 a/x_n
        Knuth 的估商公式
            用被除数最高两段和除数最高一段试根
            反过来验证余数小于除数那一段
            重复
        在保证 除数那一段 > 基数/2 时, 误差<=2
        O(n^2)  n 为段数

    Newton 除法
        迭代算 a/x_n
    解方程 f(x) = 1/x − b = 0
    Newton 法
        x_(n+1) = x_n − f(x_n) / f'(x_n)
        得 x_(n+1) = x_n * (2 - b*x_n)
        注: x0 ∈ (0, 2/b)

    设 e_k = x_k − 1/b
    求 e_(k+1) 得 e_(k+1) = -b * e_k^2
*/

#pragma once // 头文件包含保护

#include "BigNum.hpp"

#include "log2.hpp"

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

typedef unsigned short int ui16;
typedef unsigned int ui32;
typedef unsigned long long ui64;

template<ui64 Precision> BigNum<Precision> Newton(BigNum<Precision>& b) { // 计算 1/x
    static BigNum<Precision> x0 = b.approx_recip(); // 初值
    x0.replace(1); // 初值的位数
    static BigNum<Precision> x1 = x0 * ( 2 - b * x0 );
    x1.replace(2);
    ui16 TERMS = log2(Precision) + 7;

    // x_(n+1) = x_n * (2 - b*x_n)
    for (ui16 i=2; i <= TERMS; ++i) {
        x0 = x1;
        x1 = x0 * ( 2 - b * x0 );
        x1.replace(1LL << i);
    }
    return x1;
}