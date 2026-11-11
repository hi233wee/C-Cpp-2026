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
// 调参

static constexpr ui8  AI_DEPTH = 8;
static constexpr ui16 AI_LIMIT = 7;

template <cui16 SIZE>
inline sf::Vector2u AI(ui8 (&board)[SIZE][SIZE], cui8 turn)
{
	// 一定要直接修改
	Move list[SIZE * SIZE];
	i16 n = gen_moves<SIZE, AI_LIMIT>(board, turn, list);
	if (n <= 0) { return sf::Vector2u(0, 0); }

	if (AI_DEPTH <= 0) { return sf::Vector2u(list[0].x, list[0].y); }

	i16 besti = 0;
	i64 bestv = -INF - AI_DEPTH;

	for (i16 i = 0; i < n; ++i)
	{
		board[list[i].y][list[i].x] = turn;

		if (check_win(list[i].x, list[i].y, turn))
		{
			board[list[i].y][list[i].x] = 0;
			return sf::Vector2u(list[i].x, list[i].y);
		}
		else
		{
			i64 v = -negamax<SIZE, AI_LIMIT>(board, turn == 1 ? 2 : 1, -INF, INF, AI_DEPTH - 1);
			if (v > bestv) { bestv = v; besti = i; }
		}

		board[list[i].y][list[i].x] = 0;
	}
	return sf::Vector2u(list[besti].x, list[besti].y);
}
