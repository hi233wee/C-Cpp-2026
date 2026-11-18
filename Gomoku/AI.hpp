#pragma once

#include <iostream>

#include "search.hpp"
#include "TIME.hpp"

// 点打分: point_score.hpp
// 
// 给整个局面打分: board_score.hpp
// 
// 候选点生成与排序: gen_moves.hpp
// 
// 搜索: search.hpp
// 
// 调参

static constexpr ui8  AI_DEPTH = 77;
static constexpr ui16 AI_LIMIT = 12;
static constexpr ui16 AI_TIME  = 777 >> 1;

template <cui16 SIZE>
inline sf::Vector2u AI(ui8 (&board)[SIZE][SIZE], cui8 turn)
{
	// 一定要直接修改
	timer_start();
	Move list[SIZE * SIZE];
	i16 n = gen_moves<SIZE>(board, turn, AI_LIMIT, list);
	if (n <= 0) { return sf::Vector2u(0, 0); }

	if (AI_DEPTH <= 0) { return sf::Vector2u(list[0].x, list[0].y); }

	i64 eval  = eval_board<SIZE>(board, turn);
	i16 besti = 0;

	for (ui8 d = 2; d <= AI_DEPTH; d += 2) // 迭代加深(控时)
	{
		i16 tmpi = 0;
		i64 tmpv = -INF - d;

		for (i16 i = 0; i < n && now() <= AI_TIME; ++i)
		{
			board[list[i].y][list[i].x] = turn;

			if (check_win(list[i].x, list[i].y, turn))
			{
				board[list[i].y][list[i].x] = 0;
				return sf::Vector2u(list[i].x, list[i].y);
			}
			else
			{
				i64 v = -negamax<SIZE>(board, turn == 1 ? 2 : 1,
					-INF, -tmpv, -eval - eval_point<SIZE>(board, list[i].x, list[i].y, turn),
					d - 1);
				if (v > tmpv) { tmpv = v; tmpi = i; }
			}

			board[list[i].y][list[i].x] = 0;
		}
		if (now() > AI_TIME) { break; }

		besti = tmpi;
		// std::swap(list[besti], list[0]);
	}
	return sf::Vector2u(list[besti].x, list[besti].y);
}
