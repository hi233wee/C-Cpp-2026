#pragma once

#include <chrono>

typedef unsigned short int ui16;
typedef unsigned long long ui64;
using Clock = std::chrono::steady_clock;

inline bool OVER_TIME = false;
inline Clock::time_point g_time_limit = Clock::now();
inline ui16 g_nodes = 0;


inline bool intime()
{
    return !OVER_TIME; // 防止出来之后才超时被跳过
}


inline bool over_time()
{
	// 每个节点都查就太费时
    if (++g_nodes & 1023) { return OVER_TIME; }
	g_nodes = 0;
	if (Clock::now() > g_time_limit) { OVER_TIME = true; }
	return OVER_TIME;
}


template<ui16 AI_TIME>
inline void timer_start()
{
	OVER_TIME = false;
	g_time_limit = Clock::now() + std::chrono::milliseconds(AI_TIME);
}
