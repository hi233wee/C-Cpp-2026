#pragma once

#include <iostream>

#include "search.hpp"

// 点打分: point_score.hpp
// 
// 给整个局面打分: board_score.hpp
// 
// 候选点生成与排序: gen_moves.hpp
// 
// 搜索: search.hpp
// 
// 哈希: hash.hpp
// 
// 置换表: tt.hpp
// 
// 调参

static constexpr ui8  AI_DEPTH = 77;
static constexpr ui16 AI_LIMIT = 17;
static constexpr ui16 AI_TIME  = 777;


template <cui16 SIZE>
inline void init_AI()
{
	init_zobrist<SIZE>();
	init_levels<SIZE>();
	init_vals<SIZE>();
}

inline std::mt19937_64& ai_rng()
{
	static std::mt19937_64 e(std::random_device{}());
	return e;
}

template <cui16 SIZE>
inline void shuffle_ties(Move* list, i16 n) // 打乱同分候选
{
	i16 i = 0;
	while (i < n)
	{
		i16 j = i + 1;
		while (j < n && list[j].score == list[i].score) { ++j; }

		if (j - i > 1) { std::shuffle(list + i, list + j, ai_rng()); }
		i = j;
	}
}


template <cui16 SIZE>
inline sf::Vector2u AI(ui8 (&board)[SIZE][SIZE], cui8 me)
{
	timer_start<AI_TIME>();

	Move list[SIZE * SIZE];
	i16 n = gen_moves<SIZE>(board, me, AI_LIMIT, list);
	if (n <= 0) { return sf::Vector2u(0, 0); }
	shuffle_ties<SIZE>(list, n);

	// if (AI_DEPTH <= 0) { return sf::Vector2u(list[0].x, list[0].y); }

	eval_board<SIZE>(board, me);
	hash_board<SIZE>(board);

	for (ui8 d = 2; d <= AI_DEPTH; d += 2) // 迭代加深方便控时
	{
		if (TTEntry* entry = tt_probe(g_key); entry && entry->depth >= d)
		{
			for (i16 i = 0; i < n; ++i)
			{
				if (list[i].x == entry->mx && list[i].y == entry->my)
				{
					std::swap(list[0], list[i]);
					break;
				}
			}
		}

		i16 tmpi = 0;
		i64 tmpv = -INF - d;

		for (i16 i = 0; i < n && intime(); ++i)
		{
			do_move<SIZE>(board, list[i].x, list[i].y, me);

			if (check_win(list[i].x, list[i].y, me))
			{
				board[list[i].y][list[i].x] = 0;
				return sf::Vector2u(list[i].x, list[i].y);
			}
			else
			{
				i64 v = -negamax<SIZE>(board, me == 1 ? 2 : 1,
					-INF, -tmpv, d - 1, 1);
				if (v > tmpv) { tmpv = v; tmpi = i; }
			}

			undo_move<SIZE>(board, list[i].x, list[i].y, me);
		}
		if (!intime()) { break; } // 不用 over_time()

		std::swap(list[0], list[tmpi]);

		tt_store(TTEntry{ g_key, tmpv, d, list[0].x, list[0].y, TT_EXACT });
	}

	return sf::Vector2u(list[0].x, list[0].y); // 最佳都被换到 0
}
// 待办 (按投入产出排):
//   1. VCF: 只搜"冲四 -> 必应"这条强制线, 能穿 20~30 步, 补深度不够的洞
//   3. PVS: 第一个候选全窗口, 后面先用空窗口试探, 不行再重搜, 快 20~30%
//   4. 杀手着法: 每层记两手造成剪枝的着法, 优先搜; 和 TT 互补 (TT 记局面, 它记深度)
//   5. 候选宽度: 前 N 名 + 所有能成三的点, 别让必胜手被截断
