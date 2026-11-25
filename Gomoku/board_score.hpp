#pragma once

#include <iostream>

#include "point_score.hpp"          // 这里面东西很多嘛

inline i64* g_point_val = nullptr;
inline i64 g_board_val[3] = { 0 };


template <cui16 SIZE>
inline void init_vals()
{
    g_point_val = new i64[SIZE * SIZE]();
}


template <cui16 SIZE>
inline void eval_board(ui8(&board)[SIZE][SIZE], ui8 me)
{
    g_board_val[1] = g_board_val[2] = 0;

    for (i16 y = 0; y < SIZE; ++y)
        for (i16 x = 0; x < SIZE; ++x)
        {
            ui8 c = board[y][x];
            if (!c) continue;

            g_board_val[c] += eval_point<SIZE>(board, x, y, c);
        }
}


constexpr i16 bs_dirs[4][2] = { { 1,0 },{ 0,1 },{ 1,1 },{ 1,-1 } };
template <cui16 SIZE>
inline void update_line(ui8(&board)[SIZE][SIZE], i16 x, i16 y, ui8 d, ui8 me)
{
    i16 dx = bs_dirs[d][0], dy = bs_dirs[d][1];
    ui8 windows[17] = { 0 };                        // 总共十七个格
    ui8 bad = 0, cnt = 0;

    for (i16 p = -8; p <= 8; ++p)
    {
        ui8 c = cell_at<SIZE>(board, x + dx * p, y + dy * p, me);
        windows[p + 8] = c;

        if (p <= -3)
        {
            if (c == 2 && p > -8 && p < -3) { ++bad; }
            if (c == 1) { ++cnt; }
        }

        if (p >= -4 && p <= 4 && c == 1)
        {
            g_levels[d][x + dx * p + (y + dy * p) * SIZE] = LV_NONE;
        }
    }

    for (i16 i = 0; i < 12; ++i)                     // 线性扫描窗口
    {
        Level level = enlevel(windows + i, bad, cnt);

        for (i16 p = std::max(-4, i - 8); p <= 4 && p < i - 2; ++p)
        {
            if (windows[p + 8] != 1) { continue; }

            ui16 place = x + dx * p + (y + dy * p) * SIZE;
            g_levels[d][place] = std::max(g_levels[d][place], level);
        }

        if (i == 11) { break; }
        if (windows[i    ] == 1) { --cnt; }
        if (windows[i + 1] == 2) { --bad; }
        if (windows[i + 5] == 2) { ++bad; }
        if (windows[i + 6] == 1) { ++cnt; }
    }

    for (i16 p = -4; p <= 4; ++p)
    {
        if (windows[p + 8] != 1) { continue; }

        ui16 place = x + dx * p + (y + dy * p) * SIZE;
        Level L[4] = { g_levels[0][place],
                       g_levels[1][place],
                       g_levels[2][place],
                       g_levels[3][place] };

        g_board_val[me] -= g_point_val[place];
        g_point_val[place] = combo_score(L);
        g_board_val[me] += g_point_val[place];
    }
}


// 依旧神力
template <cui16 SIZE>
inline void update_point(ui8(&board)[SIZE][SIZE], i16 x, i16 y)
{
    for (ui8 d = 0; d < 4; ++d)
    {
        update_line<SIZE>(board, x, y, d, 1);
        update_line<SIZE>(board, x, y, d, 2);
    }
    if (board[y][x]) { return; }
}