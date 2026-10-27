/*
    1.Machin  (除法多, 难优化)

    π/4 = 4*arctan(1/5) - arctan(1/239)

    泰勒(用于收敛): arctan x = ∑{i=0} (-1)^i * x^(2i+1) / (2i+1)
*/

/*
    2.Chudnovsky  (我用的)

    ​1/π = 12*∑{i=0} [ (-1)^i * (6i)! * (13591409+545140134k) ]
                    / [ (3i)! * (i!)^3 * 640320^(3i+3/2) ]

    分开一些:
    M_i = (6i)! / [ (3i)! * (i!)^3 ]
    L_i = 13591409 + 545140134*i
    X_i = (640320^3)^i
    (-1)^i

    S_i = M_i * L_i / X_i * (-1)^i

    递推算 M L X

    最后 π = 426880 * (10005)^(1/2) / ∑S_i

    使用 binary splitting 二分算 ∑S_i

    使用 NTT 算大数相乘

    binary splitting 具体应用
        M_i / M_(i-1) = (6i)(6i-1)(6i-2)(6i-3)(6i-4)(6i-5) / [ (3i)(3i-1)(3i-2) * i^3 ]
                      = 24(6i-1)(2i-1)(6i-5) / i^3
        X_i / X_(i-1) = -1 * 640320^3
        设 p_k = (6k-1)(2k-1)(6k-5)
           q_k = k^3 * 640320^3 / 24
        则 S_k = ∏{i=1}{k} (p_i / q_i) * L_k * (-1)^k
        通分 S 得 T = ∑{k=0}{n-1} ∏{i=1}{k} p_i * ∏{i=k+1}{n-1} q_i * L_k * (-1)^k
        对 T 二分 ( a < m < b )
        T[a,b] = ∑{k=a  }{b} ∏{i=a  }{k} p_i * ∏{i=k+1}{b} q_i * L_k * (-1)^k

               = ∑{k=a  }{m} ∏{i=a  }{k} p_i * ∏{i=k+1}{b} q_i * L_k * (-1)^k
                +∑{k=m+1}{b} ∏{i=a  }{k} p_i * ∏{i=k+1}{b} q_i * L_k * (-1)^k

        T[a,m] = ∑{k=a  }{m} ∏{i=a  }{k} p_i * ∏{i=k+1}{m} q_i * L_k * (-1)^k
        T(m,b] = ∑{k=m+1}{b} ∏{i=m+1}{k} p_i * ∏{i=k+1}{b} q_i * L_k * (-1)^k

        所以 T[a,b] = T[a,m] * ∏{i=m+1}{b} q_i + T(m,b] * ∏{i=a}{m} p_i

    而 binary splitting 可以多线程
*/

#pragma once // 头文件包含保护

// 让头文件知道BASES //
#include "BigNum.hpp" // 里面啥都有

#include "NTT.hpp"
#include "isqrt.hpp"
#include "Newton.hpp"

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

typedef unsigned short int ui16;
typedef unsigned int ui32;
typedef unsigned long long ui64;

template<ui64 Precision> BigNum<Precision> Chudnovsky() {
    static BigNum<Precision> sqrt_10005 = 10005;
    sqrt_10005 = isqrt<Precision>(sqrt_10005);
    static ui16 TERMS = Precision / 14.0L;    // Chudnovsky 项数

    static BigNum<Precision> pi = sqrt_10005 * 426880LL / binary_splitting<Precision>(1, TERMS);

    return pi;
}

template<ui64 Precision> BigNum<Precision> get_PI() {
    init_pick<Precision>(); // 必须先用一次

    return Chudnovsky<Precision>();
}