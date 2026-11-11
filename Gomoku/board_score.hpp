#pragma once

#include <iostream>

#include "point_score.hpp"          // 这里面东西很多嘛

constexpr i64 LINE_VAL[8] =         // 下标就是 Level 的值
{ 0, S_S2, S_O2, S_S3, S_O3, S_S4, S_O4, S_5 };


template <cui16 SIZE>
inline i64 eval_board(ui8(&board)[SIZE][SIZE], ui8 me)
{
    i64 sc[3] = { 0, 0, 0 };

    for (i16 y = 0; y < SIZE; ++y)
        for (i16 x = 0; x < SIZE; ++x)
        {
            ui8 c = board[y][x];
            if (!c) continue;

            for (auto d : ps_dirs)
            {
                i16 dx = d[0], dy = d[1];
                i16 px = x - dx, py = y - dy;
                if (px >= 0 && px < SIZE && py >= 0 && py < SIZE && board[py][px] == c)
                {
                    continue;       // 不是段首, 跳过
                }

                sc[c] += LINE_VAL[line_level<SIZE>(board, x, y, dx, dy, c)];
                // 这里有个问题: 跳型会被重复, 但一般不影响
            }
        }
    return sc[me] - sc[me == 1 ? 2 : 1];
}
// 搜索到第 4 层时, 双活三 已经被搜索出来了, 其他更低的棋型本来就没什么分