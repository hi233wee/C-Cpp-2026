#pragma once

#include <iostream>

#include "point_score.hpp"          // 这里面东西很多嘛


template <cui16 SIZE>
inline i64 eval_board(ui8(&board)[SIZE][SIZE], ui8 me)
{
    i64 sc[3] = { 0, 0, 0 };

    for (i16 y = 0; y < SIZE; ++y)
        for (i16 x = 0; x < SIZE; ++x)
        {
            ui8 c = board[y][x];
            if (!c) continue;

            sc[c] += eval_point<SIZE>(board, x, y, c);
            // 为了方便局面分传递
        }
    return sc[me] - sc[me == 1 ? 2 : 1];
}
