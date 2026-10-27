/*
    大数类

    存 10^W 进制的 N 段大整数, W 和 N 在编译期确定
    每段只装 ui32
    左边低位, 右边高位
    优先保证高位

    开根那先化为整数
    最后除得整数
    手动加点
*/

#pragma once // 头文件包含保护

#include <iostream>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <string>

typedef unsigned short int ui16;
typedef unsigned int ui32;
typedef unsigned long long ui64;
typedef unsigned __int128 ui128;

#define M1 998244353LL
#define M2 1004535809LL

static ui64 BASES[19] = { 1LL, 10LL, 100LL, 1LL<<3, 1LL<<4, 1LL<<5, 1LL<<6, 1LL<<7, 1LL<<8, 1LL<<9,
                        1LL<<10, 1LL<<11, 1LL<<12, 1LL<<13, 1LL<<14, 1LL<<15, 1LL<<16, 1LL<<17, 1LL<<18 };

static ui16 W;        // 每段位数
static ui16 BASE;     // 每段基数
static ui32 N;        // 段数

template<ui64 Precision> constexpr void init_pick() {
    ui16 best_W = 0;
    ui64 best_cost = 1e18, cost = 0;
    ui128 maxNum = 0;
    for (W = 9; W >= 1; --W) {
        N = static_cast<long double>(Precision) / W + 2;
        maxNum = static_cast<ui128>(N) * BASES[W<<1]; // NTT 的最大数
        if (maxNum > M1 * M2) { continue; }
        
        cost = N * 3; // 两模正变换+逆变换
        if (cost < best_cost) {
            best_cost = cost;
            best_W = W;
        }
    }
    W = best_W;
    N = Precision / 14.0L * 17 / W + 7; // 中间乘法多几段
    BASE = BASES[W];
}

template<ui64 Precision> class BigNum {
private:
    ui32* num = new ui32[N](); // 整数数组
    ui32 hi = 0, lo = 0; // 最高位和最低位的下标(去掉前导零)
    // ui32 point = 0; // 小数位数
    bool flag = 0; // 符号 0正
public:
    ~BigNum() { delete[] num; }
    void clear() noexcept {
        delete[] num;
        num = nullptr;
        hi = 0;
        lo = 0;
        // point = 0;
        flag = 0;
    }

    
    BigNum<Precision>& update() {
        hi = N - 1;
        lo = 0;
        while (hi > 0  && num[hi] == 0) { --hi; }
        while (lo < hi && num[lo] == 0) { ++lo; }
        return *this;
    }
    BigNum<Precision>& pow10(ui64& P) {
        //
    }
    /*void shl(ui32& P) {
        if (P > point) {
            pow10(P - point);
            point = 0;
        } else { point -= P; }
    }
    void shr(ui32& P) {
        point += P;
    }*/
    BigNum<Precision>& negate() {
        flag = !flag;
        return *this;
    }
    friend BigNum<Precision>& negate(BigNum<Precision>& a) {
        BigNum<Precision> tmp = a;
        tmp.flag = !tmp.flag;
        return tmp;
    }

    // 构造函数 //
    BigNum() noexcept {}
    BigNum(const ui64& a) noexcept {
        ui64 tmp = a;

        for (ui32 i = 0; i < N && tmp > 0; ++i) {
            num[i] = tmp % BASE;
            tmp /= BASE;
        }
        update();
    }
    BigNum(const BigNum<Precision>& a) noexcept {
        std::copy(a.num, a.num + N, num);
        hi = a.hi;
        lo = a.lo;
        // point = a.point;
        flag = a.flag;
    }
    BigNum(BigNum<Precision>&& a) noexcept {
        std::swap(num, a.num);
        std::swap(hi, a.hi);
        std::swap(lo, a.lo);
        // std::swap(point, a.point);
        std::swap(flag, a.flag);
    }

    // 重载运算符 = //
    BigNum<Precision>& operator = (const ui64& a) {
        ui64 tmp = a;

        for (ui32 i = 0; i < N && tmp > 0; ++i) {
            num[i] = tmp % BASE;
            tmp /= BASE;
        }
        return this->update();
    }
    BigNum<Precision>& operator = (const BigNum<Precision>& a) {
        std::copy(a.num, a.num + N, num);
        hi = a.hi;
        lo = a.lo;
        // point = a.point;
        flag = a.flag;
        return *this;
    }
    

    // 重载运算符 + //
    friend BigNum<Precision>& operator + (const BigNum<Precision>& a, const ui64& b) {
        if (a.flag == 1) { return b - negate(a); }

        BigNum<Precision> result;
        ui64 carry = b, sum = 0;

        for (ui32 i = 0; i < N && carry > 0; ++i) {
            sum = a.num[i] + carry;
            result.num[i] = sum % BASE;
            carry = sum / BASE;
        }
        if (carry) {
            result.pow10(-W);
            result[N-1] = carry;
        }

        return result.update();
    }
    friend BigNum<Precision>& operator + (const ui64& b, const BigNum<Precision>& a) {
        return a + b;
    }
    friend BigNum<Precision>& operator + (const BigNum<Precision>& a, const BigNum<Precision>& b) {
        if (a.flag && b.flag) { return (negate(a) + negate(b)).negate(); }
        if (a.flag) { return b - negate(a); }
        if (b.flag) { return a - negate(b); }

        BigNum<Precision> result;
        ui64 carry = 0, sum = 0;

        for (ui32 i = std::min(a.lo, b.lo); i < N && carry > 0; ++i) {
            sum = a.num[i] + b.num[i] + carry;
            result.num[i] = sum % BASE;
            carry = sum / BASE;
        }
        if (carry) {
            result.pow10(-W);
            result[N-1] = carry;
        }

        return result.update();
    }

    // 重载运算符 - //
    friend BigNum<Precision> operator - (const BigNum<Precision>& a, const ui64 b) {
        if (a.flag) { return (negate(a) + b).negate(); }

        BigNum<Precision> result;
        ui64 borrow = 0, divisor = b;
        
        for (ui32 i = 0; i <= result.hi && borrow > 0; ++i) {
            if (a.num[i] < divisor % BASE + borrow) {
                result.num[i] = a.num[i] + BASE - divisor % BASE - borrow; // 不会超过 ui32
                divisor /= BASE;
                borrow = 1;
            } else {
                result.num[i] = a.num[i] % BASE;
                borrow = 0;
            }
        }
        
        return result.update();
    }
    friend BigNum<Precision> operator - (const ui64 b, const BigNum<Precision>& a) {
        if (a.flag) { return negate(a) + b; }

        BigNum<Precision> result(b);
        return result - a;
    }
    friend BigNum<Precision> operator - (const BigNum<Precision>& a, const BigNum<Precision>& b) {
        if (a.flag && b.flag) { return negate(b) - negate(a); }
        if (a.flag) { return (negate(a) + b).negate(); }
        if (b.flag) { return a + negate(b); }
        if (a.hi < b.hi) { return (b - a).negate(); }

        BigNum<Precision> result;
        ui32 borrow = 0;
        result.lo = std::min(a.lo, b.lo);
        result.hi = std::max(a.hi, b.hi);

        for (ui32 i = result.lo; i <= result.hi; ++i) {
            if (a.num[i] < b.num[i] + borrow) {
                result.num[i] = a.num[i] + BASE - b.num[i] - borrow;
                borrow = 1;
            } else {
                result.num[i] = a.num[i] - b.num[i] - borrow;
                borrow = 0;
            }
        }

        return result.update();
    }

    // 重载运算符 * //
    friend BigNum<Precision> operator * (const BigNum<Precision>& a, const ui64 b) {
        BigNum<Precision> result;
        ui128 carry = 0, product = 0;

        for (ui32 i = a.lo; i <= a.hi; ++i) {
            product = static_cast<ui128>(a.num[i]) * b + carry;
            result.num[i] = product % BASE;
            carry = product / BASE;
        }
        while (carry > 0) {
            if (result.hi == N-1) { result.pow10(-W); --result.hi; }
            result.num[++result.hi] = carry % BASE;
            carry /= BASE;
        }

        return result.update();
    }
    friend BigNum<Precision> operator * (const ui64 b, const BigNum<Precision>& a) {
        return a * b;
    }

    // 重载运算符 / //
    friend BigNum<Precision> operator / (const BigNum<Precision>& a, const ui64 b) {
        BigNum<Precision> result;





        result.update();
        return result;
    }

    // 输出 //
    friend std::ostream& operator << (std::ostream& os, const BigNum<Precision>& bn) {
            //
        return os;
    }
};

template<ui64 Precision> BigNum<Precision> make_BigNum(const ui64& a) {
    BigNum<Precision> tmp(a);
    return tmp;
}
template<ui64 Precision> BigNum<Precision> make_BigNum(const BigNum<Precision>& a) {
    BigNum<Precision> tmp(a);
    return tmp;
}
template<ui64 Precision> BigNum<Precision> make_BigNum(BigNum<Precision>&& a) {
    BigNum<Precision> tmp(a);
    return tmp;
}

#undef ui64
#undef ui32
#undef ui16