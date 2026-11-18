#pragma once

#include "board_score.hpp"
#include "gen_moves.hpp"
#include "TIME.hpp"

constexpr i64 INF = LLONG_MAX >> 2;


template <cui16 SIZE>
inline i64 negamax(ui8(&board)[SIZE][SIZE], ui8 me, i64 alpha, i64 beta, i64 eval, ui8 depth)
{
	if (depth == 0) { return eval; }
	
	Move list[SIZE * SIZE];
	i16 n = gen_moves<SIZE>(board, me, depth + 2, list);
	if (n <= 0) { return 0; }

	i64 best = -INF - depth;

	for (i16 i = 0; i < n; ++i)
	{
		board[list[i].y][list[i].x] = me;

		if (check_win(list[i].x, list[i].y, me))
		{
			board[list[i].y][list[i].x] = 0;
			return INF + depth;	// 早点赢好
		}

		i64 v = -negamax<SIZE>(board, me == 1 ? 2 : 1,
			-beta, -alpha, -eval - eval_point<SIZE>(board, list[i].x, list[i].y, me),
			depth - 1);

		board[list[i].y][list[i].x] = 0;

		if (v > alpha) { alpha = v; }
		if (alpha >= beta) { best = beta; break; }
		if (v > best) { best = v; }
	}
	return best;
}
// alpha : 我能得到的分数下界
// beta  : 对手能让我得到的分数上界
// negamax : me 在 depth 手后能得到最的分

// 剪枝: 如果 best >= beta, 说明对手在之前有更好的选择


// 没写  哈希置换表 + 最佳手优先
