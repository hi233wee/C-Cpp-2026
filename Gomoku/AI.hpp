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
static constexpr ui16 AI_LIMIT = 12;
static constexpr ui16 AI_TIME  = 777 >> 1;


template <cui16 SIZE>
inline void init_AI()
{
	init_zobrist<SIZE>();
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
inline sf::Vector2u AI(ui8 (&board)[SIZE][SIZE], cui8 turn)
{
	timer_start<AI_TIME>();

	Move list[SIZE * SIZE];
	i16 n = gen_moves<SIZE>(board, turn, AI_LIMIT, list);
	if (n <= 0) { return sf::Vector2u(0, 0); }
	shuffle_ties<SIZE>(list, n);

	// if (AI_DEPTH <= 0) { return sf::Vector2u(list[0].x, list[0].y); }

	i64 eval  = eval_board<SIZE>(board, turn);
	ui64 key  = hash_board<SIZE>(board);
	i16 besti = 0;

	for (ui8 d = 2; d <= AI_DEPTH; d += 2) // 迭代加深方便控时
	{
		i16 tmpi = 0;
		i64 tmpv = -INF - d;

		for (i16 i = 0; i < n && intime(); ++i)
		{
			board[list[i].y][list[i].x] = turn;
			key ^= g_z[turn - 1][list[i].y * SIZE + list[i].x];

			if (check_win(list[i].x, list[i].y, turn))
			{
				board[list[i].y][list[i].x] = 0;
				return sf::Vector2u(list[i].x, list[i].y);
			}
			else
			{
				i64 v = -negamax<SIZE>(board, turn == 1 ? 2 : 1,
					-INF, -tmpv,
					-eval - list[i].my_score, key,
					d - 1);
				if (v > tmpv) { tmpv = v; tmpi = i; }
			}

			board[list[i].y][list[i].x] = 0;
			key ^= g_z[turn - 1][list[i].y * SIZE + list[i].x];
		}
		if (!intime()) { break; } // 不用 over_time()

		besti = tmpi;
		tt_store(TTEntry{ key, tmpv, d, list[besti].x, list[besti].y, TT_EXACT });
	}

	return sf::Vector2u(list[besti].x, list[besti].y);
}
