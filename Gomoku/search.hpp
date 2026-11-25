#pragma once

#include "board_score.hpp"
#include "gen_moves.hpp"
#include "tt.hpp"
#include "hash.hpp"
#include "try_move.hpp"
#include "TIME.hpp"

constexpr i64 INF = LLONG_MAX >> 2;


template <cui16 SIZE>
inline i64 negamax(ui8(&board)[SIZE][SIZE], ui8 me, i64 alpha, i64 beta, ui8 depth, ui8 ply)
{
	if (depth == 0) { return g_board_val[me] - g_board_val[me == 1 ? 2 : 1]; }

	Move list[SIZE * SIZE];
	i16 n = gen_moves<SIZE>(board, me, depth + 2, list);
	if (n <= 0) { return 0; }

	if (TTEntry* entry = tt_probe(g_key); entry && entry->depth >= depth)
	{
		if (entry->flag == TT_EXACT) { return entry->score; }
		if (entry->flag == TT_LOWER) { alpha = std::max(alpha, entry->score); }
		if (entry->flag == TT_UPPER) {  beta = std::min( beta, entry->score); }
		if (alpha >= beta) { return entry->score; }

		for (i16 i = 0; i < n; ++i)
		{
			if (list[i].x == entry->mx && list[i].y == entry->my)
			{
				std::swap(list[0], list[i]);
				break;
			}
		}
	}

	if (over_time()) { return 0; }

	i64 bestv = -INF + ply;
	i64 besti = 0;
	bool nobigger = true;

	for (i16 i = 0; i < n; ++i)
	{
		board[list[i].y][list[i].x] = me;

		if (check_win(list[i].x, list[i].y, me))
		{
			board[list[i].y][list[i].x] = 0;
			return INF - ply;	// 早点赢好
		}

		do_move<SIZE>(board, list[i].x, list[i].y, me);

		i64 v = -negamax<SIZE>(board, me == 1 ? 2 : 1,
			-beta, -alpha, depth - 1, ply + 1);

		undo_move<SIZE>(board, list[i].x, list[i].y, me);

		if (v > bestv) { bestv = v; besti = i; }
		if (v > alpha) { alpha = v; nobigger = false; }
		if (alpha >= beta) { break; }
	}

	// bestv <= alpha: 搜完得到上界, 子节点剪枝会返回 -alpha 的值
	//				   又因为会改 alpha, 所以用 nobigger
	// bestv >=  beta: 剪枝了, 得到下界
	// 
	// alpha < bestv < beta: 搜完得到精确值
	tt_store(TTEntry{ g_key, bestv, depth, list[besti].x, list[besti].y,
		nobigger ? TT_UPPER : bestv >= beta ? TT_LOWER : TT_EXACT });

	return bestv;
}
// alpha : 我能得到的分数下界
//  beta : 对手能让我得到的分数上界
// 剪枝: 如果 alpha >= beta, 说明对手在之前有更好的选择
// 
// negamax : me 在 depth 手后能得到最的分
