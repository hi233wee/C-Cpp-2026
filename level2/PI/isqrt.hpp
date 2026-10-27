/*
    迭代
    x_(n+1) = (1/2)(x_n + a/x_n)
    a^(1/2) = lim{n->+∞} x_n
    注: x0 > a^(1/2)

    设 e_k = x_k − a^(1/2)
    求 e_(k+1) 得 e_(k+1) = (1/2)( e_k^2 / ( e_k + a^(1/2) ) )
*/

#pragma once // 头文件包含保护

#include "BigNum.hpp"

#include "log2.hpp"

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

typedef unsigned short int ui16;
typedef unsigned long long ui64;

template<ui64 Precision> BigNum<Precision> isqrt(BigNum<Precision>& a) {
    if (a.flag) { throw std::invalid_argument("ui64_sqrt: a < 0"); }
    if (a.equal(0)) { return BigNum<Precision>(0); }
    if (a.equal(1)) { return BigNum<Precision>(1); }
    
    BigNum<Precision> x = a.approx_sqrt(); // 初值
    // 补化整
    ui16 TERMS = log2(Precision) + 1; // 迭代次数

    // x_(n+1) = (1/2)(x_n + a/x_n)
    for (ui16 i = 0; i < TERMS; ++i) {
        x = ( x + a * Newton<Precision>(x) ) / 2; // 补位判断
    }
    return x;
}

#undef ui64
#undef ui16