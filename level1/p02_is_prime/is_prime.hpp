// 判断素数 //
// 米勒拉宾素性测试(n<2^64)

#include <cstdio>
#include <windows.h>
#include <conio.h>
using namespace std;
typedef unsigned long long uLL;

uLL base[]={2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};

uLL multiply_mod (uLL a, uLL b, uLL mod) { //-----------a*b % mod
    return (unsigned __int128)a*b %mod;
}

uLL power_mod (uLL a, uLL b, uLL mod) { //--------------a^b % mod(快速幂)
    uLL x=1;
    while (b) { // 类似二进制
        if (b&1) { x=multiply_mod(x, a, mod); }
        a=multiply_mod(a, a, mod);
        b>>=1;
    }
    return x;
}

bool is_prime (uLL n) { //------------------------------判断素数
    // 费马小定理:
    // 若p为素数, a与p互质, 则a^(p-1) %p ≡ 1.
    // 米勒-拉宾在它基础上加了二次探方: 若 x² ≡ 1 (mod p), 则 x ≡ ±1 (mod p)
    // 当底数合适时, 可逆用判断一定范围内的素数
    if (n<2)    { return false; }
    if (n==2)   { return true ; }
    if (n%2==0) { return false; }
    // 先把n-1分离成d * 2^s //
    uLL d=n-1; int s=0;
    while (d%2==0) { d>>=1; s++; }
    // 判断 //
    for (int i=0; i<12; i++) {
        if (base[i]>=n) { break; } // 底数大
        // a^d //
        uLL x=power_mod(base[i], d, n);
        if (x==1 || x==n-1) { continue; }
        // a^(d* 2^k) //
        bool o =false;
        for (int j=1; j<s; j++) {
            x=multiply_mod(x, x, n);
            if (x==n-1) { o=true; break; }
        }
        if (!o) { return false; }
    }
    return true;
}
