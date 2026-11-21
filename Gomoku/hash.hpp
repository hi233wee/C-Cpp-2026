#pragma once

#include <random>
#include <cstring>

#include "point_score.hpp"

typedef unsigned long long ui64;


inline ui64* g_z[2];

template <cui16 SIZE>
inline void init_zobrist()
{
	g_z[0] = new ui64[SIZE * SIZE];
	g_z[1] = new ui64[SIZE * SIZE];
	std::mt19937_64 rng(7777777); // 没有问题, 放心使用

	for (i16 y = 0; y < SIZE; ++y)
		for (i16 x = 0; x < SIZE; ++x)
			for (ui8 c = 0; c < 2; ++c)
				g_z[c][y * SIZE + x] = rng();
}

/*
// 异或两次复原
template <cui16 SIZE>
inline void hash_toggle(ui64& h, i16 x, i16 y, ui8 color)
{
	h ^= g_z[color - 1][y * SIZE + x]; // 黑 0, 白 1
}*/


// 只在开局或校验时用
template <cui16 SIZE>
inline ui64 hash_board(ui8 (&board)[SIZE][SIZE])
{
    ui64 h = 0;
    for (i16 y = 0; y < SIZE; ++y)
        for (i16 x = 0; x < SIZE; ++x)
			if (board[y][x]) { h ^= g_z[board[y][x] - 1][y * SIZE + x]; }
    return h;
}
