#pragma once

#include "point_score.hpp"

// 候选点
struct Move
{
	i16 x;
	i16 y;
	i64 score;
};


// 周围 r 格内有没有棋子
template <cui16 SIZE>
inline bool near_stone(ui8 (&board)[SIZE][SIZE], i16 x, i16 y, i16 r = 2)
{
	for (i16 dy = -r; dy <= r; ++dy)
	{
		for (i16 dx = -r; dx <= r; ++dx)
		{
			i16 nx = x + dx, ny = y + dy;
			if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && board[ny][nx]) return true;
		}
	}
	return false;
}


// 候选点  我的收益 + 他的收益  只搜索前 limit 个
template <cui16 SIZE, cui16 limit = 7>
inline i16 gen_moves(ui8 (&board)[SIZE][SIZE], ui8 me, Move (&out)[SIZE * SIZE])
{
	i16 n = 0;

	for (i16 y = 0; y < SIZE; ++y)
	{
		for (i16 x = 0; x < SIZE; ++x)
		{
			if (board[y][x]) continue;
			if (!near_stone<SIZE>(board, x, y)) continue;

			i64 mine = eval_point<SIZE>(board, x, y, me);
			i64 opp  = eval_point<SIZE>(board, x, y, me == 1 ? 2 : 1);

			i64 s;
			if (mine >= S_5)     s = S_5 * 3;		// 我能成五,最高优先
			else if (opp >= S_5) s = S_5 * 2;		// 必须堵
			else                 s = mine + opp;

			out[n].x = x;
			out[n].y = y;
			out[n].score = s;
			++n;
		}
	}

	if (n == 0)										// 空盘:下天元
	{
		i16 cx = SIZE / 2, cy = SIZE / 2;
		if (board[cy][cx] == 0)
		{
			out[0].x = cx;
			out[0].y = cy;
			out[0].score = 0;
			return 1;
		}
		return 0;
	}

	i16 keep = (n < limit) ? n : limit;

	for (i16 i = 0; i < keep; ++i)					// 写 sort 还可能中弹
	{
		i16 best = i;
		for (i16 j = i + 1; j < n; ++j)
		{
			if (out[j].score > out[best].score) best = j;
		}
		if (best != i)
		{
			Move t = out[i];
			out[i] = out[best];
			out[best] = t;
		}
	}

	return keep;
}
