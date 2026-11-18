#pragma once

#include "board_score.hpp"
#include "gen_moves.hpp"
#include "tt.hpp"
#include "hash.hpp"
#include "TIME.hpp"

constexpr i64 INF = LLONG_MAX >> 2;


template <cui16 SIZE>
inline i64 negamax(ui8(&board)[SIZE][SIZE], ui8 me, i64 alpha, i64 beta, i64 eval, ui64 key, ui8 depth)
{

	if (depth == 0) { return eval; }

	if (TTEntry* entry = tt_probe(key); entry && entry->depth >= depth)
	{
		if (entry->flag == TT_EXACT) { return entry->score; }
		//else if (entry->flag == TT_LOWER) { alpha = std::max(alpha, entry->score); }
		//else if (entry->flag == TT_UPPER) { beta  = std::min(beta , entry->score); }
		//if (alpha >= beta) { return entry->score; }
	}

	if (over_time()) { return 0; }
	
	Move list[SIZE * SIZE];
	i16 n = gen_moves<SIZE>(board, me, depth + 2, list);
	if (n <= 0) { return 0; }

	i64 bestv = -INF - depth;
	i64 besti = 0;

	for (i16 i = 0; i < n; ++i)
	{
		board[list[i].y][list[i].x] = me;
		key ^= g_z[me - 1][list[i].y * SIZE + list[i].x];

		if (check_win(list[i].x, list[i].y, me))
		{
			board[list[i].y][list[i].x] = 0;
			return INF + depth;	// 早点赢好
		}

		i64 v = -negamax<SIZE>(board, me == 1 ? 2 : 1,
			-beta, -alpha,
			-eval - eval_point<SIZE>(board, list[i].x, list[i].y, me), key,
			depth - 1);

		board[list[i].y][list[i].x] = 0;
		key ^= g_z[me - 1][list[i].y * SIZE + list[i].x];

		if (v > alpha) { alpha = v; }
		if (alpha >= beta) { bestv = beta; besti = -1; break; }
		if (v > bestv) { bestv = v; besti = i; }
	}

	if (besti >= 0)
	{
		tt_store(TTEntry{key, bestv, depth, list[besti].x, list[besti].y, TT_EXACT});
	}

	return bestv;
}
// alpha : 我能得到的分数下界
// beta  : 对手能让我得到的分数上界
// negamax : me 在 depth 手后能得到最的分

// 剪枝: 如果 best >= beta, 说明对手在之前有更好的选择


// 没写  哈希置换表 + 最佳手优先
