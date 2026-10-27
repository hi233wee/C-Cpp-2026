/*
    NTT (Number Theoretic Transform 数论变换)

    先说FFT

    对于 x 的多项式 A = a_0 + a_1 * x + …… + a_(n-1) * x^(n-1)
                    B = b_0 + b_1 * x + …… + b_(n-1) * x^(n-1)
    想求 C = A·B 的系数

    普通地算卷积 c_k = ∑a_i*b_(k-i) , i∈[0,k]   O(n^2)

    但是有取 n 个点求 A(x_i) B(x_i)     O(n^2)
        计算点积 C(x_i)                 O(n)
        然后逆求 C 的系数               O(n^2)

    还是慢, 于是用蝶形算法

    设单位根 ω = e^(2πi/n)  ( i 为虚数单位)
        有 w^(k + n/2) = -w^k (欧拉公式 e^(iθ) = cosθ + isinθ) (用泰勒证欧拉)
        就可以用二分啦      O(n log n)
        就是用 Cooley-Tukey

    用浮点表示单位根(一个实部一个虚部)
        计算 C(ω^k)
    然后进行 离散傅里叶变换
    就是 FFT (Fast Fourier Transform 快速傅里叶变换)

    又因为浮点精度问题, 不能算太大

    于是换一个整数单位根 ω = g^((p-1)/n)
        原根 g 为模 p 下一个特殊整数
        它的幂(1 ~ p-1)能恰好遍历 1 ~ p−1 的所有数，且一个不重复
        于是又有 ω^(k + n/2) ≡ -ω^k (mod p)
    后面和 FFT 差不多, 但是要还原 C(ω^k)  (因为mod p)

    中国剩余定理
        c mod p1 = r1
        c mod p2 = r2
        当 c < p1*p2 时
        r1,r2,p1,p2 唯一确定 c
        (可以扩展到 n 个模)
        (p_i 两两互质)

    诶, 怎么求 c 呢?
    费马小定理
        p 为质数, 则 a^(p-1) ≡ 1 (mod p)
    所以 p1^(p2-2) ≡ p1^(-1) (mod p2)
    因为 c mod p1 = r1
        设 c = r1 + p1*k
        则 p1*k ≡ r2 - r1 + p2 (mod p2)
        乘上 p1^(-1) ≡ p1^(p2-2) (mod p2)
        求得 k ( < p2 )

    离散傅里叶变换
        正变换 A_j = ∑{i=0} a_i * ω^(ij)
        逆变换 a_k = (1/n)∑{j=0} A_j * ω^(-jk)
        证明自己代入讨论

    最后, 每个系数就是大数乘之后对应段的数
        但是还要处理进位

    诶, 可以给每个模数的计算创一个线程
*/

#pragma once // 头文件包含保护

#include "BigNum.hpp"

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>

#define M1 998244353LL
#define M2 1004535809LL
//#define M3 469762049LL // 三模要 ui128 不想写

