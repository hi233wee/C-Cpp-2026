#pragma once

#include "board_score.hpp"
#include "gen_moves.hpp"

constexpr i64 INF = LLONG_MAX >> 2;


template <cui16 SIZE, cui16 limit>
inline i64 negamax(ui8(&board)[SIZE][SIZE], ui8 me, i64 alpha, i64 beta, ui8 depth)
{
	if (depth == 0) { return eval_board<SIZE>(board, me); }
	
	Move list[SIZE * SIZE];
	i16 n = gen_moves<SIZE, limit>(board, me, list);
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

		i64 v = -negamax<SIZE, limit>(board, 3 - me, -beta, -alpha, depth - 1);

		board[list[i].y][list[i].x] = 0;

		if (v >= beta) { best = beta; break; }
		if (v > best) { best = v; }
	}
	return best;
}
// alpha : 我能得到的分数下界
// beta  : 对手能让我得到的分数上界
// negamax : me 在 depth 手后能得到最的分

// 剪枝: 如果 best >= beta, 说明对手在之前有更好的选择


// 1. 迭代加深 + 上轮最佳手优先（最推荐）
// 2. 宽度递减
// 3. eval_board 改增量
// 4. 置换表

// 1. 静态延伸（quiescence）—— 最对症
//		只对“强制手”延伸，层数有界（一般多 2~4 层），成本可控
// 2. 让 eval 也认组合
//		叶节点对“双方最强的那一手”各跑一次 eval_point
