#pragma once

#include <cstring>

#include "point_score.hpp"

typedef unsigned long long ui64;

constexpr ui64 TT_SIZE = 1 << 18;

enum TTFlag : ui8 { TT_NONE = 0, TT_EXACT, TT_LOWER, TT_UPPER };


struct TTEntry
{
    ui64 key  = 0;
    i64 score = 0;
    i16 depth = -1;
    i16 mx    = -1;
    i16 my    = -1; // -1 为空
    ui8 flag  = TT_NONE;
};

inline TTEntry g_tt[TT_SIZE];


inline TTEntry* tt_slot(ui64 h)
{
    return &g_tt[h & (TT_SIZE - 1)];
}


// 查表
inline TTEntry* tt_probe(ui64 h)
{
    TTEntry* e = tt_slot(h);
    return (e->depth >= 0 && e->key == h) ? e : nullptr;
}


inline void tt_store(TTEntry entry)
{
    TTEntry* e = tt_slot(entry.key);
	if (e->depth > entry.depth) { return; } // 深度更浅的不要覆盖

    *e = entry;
}


inline void tt_clear()
{
    std::memset(g_tt, 0, sizeof(g_tt));
    for (int i = 0; i < TT_SIZE; ++i) { g_tt[i].depth = -1; }
    return;
}
