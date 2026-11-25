#pragma once

#include "board_score.hpp"
#include "hash.hpp"


template <cui16 SIZE>
inline void do_move(ui8 (&board)[SIZE][SIZE], i16 x, i16 y, ui8 me)
{
	board[y][x] = me;
	g_key ^= g_z[me - 1][y * SIZE + x];
	update_point<SIZE>(board, x, y);
}


template <cui16 SIZE>
inline void undo_move(ui8(&board)[SIZE][SIZE], i16 x, i16 y, ui8 me)
{
	ui16 place = x + y * SIZE;
	g_board_val[me] -= g_point_val[place];
	g_point_val[place] = 0;
	for (ui8 d = 0; d < 4; ++d) { g_levels[d][place] = LV_NONE; }

	board[y][x] = 0;
	g_key ^= g_z[me - 1][y * SIZE + x];
	update_point<SIZE>(board, x, y);
}