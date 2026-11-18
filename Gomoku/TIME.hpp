#pragma once

#include <chrono>

typedef unsigned long long ui64;


using Clock = std::chrono::steady_clock;

inline Clock::time_point g_time_start = Clock::now();


// 从 timer_start() 到现在过了多少毫秒
inline ui64 now()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now() - g_time_start).count();
}


inline void timer_start()
{
    g_time_start = Clock::now();
}
