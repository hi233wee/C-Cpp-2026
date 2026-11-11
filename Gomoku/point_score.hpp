#pragma once

#include <algorithm>

typedef short int i16;
typedef long long i64;
typedef const short int ci16;
typedef unsigned char ui8;
typedef unsigned short int ui16;
typedef const unsigned char cui8;
typedef const unsigned short int cui16;


// 棋型等级
enum Level
{
    LV_NONE, LV_S2, LV_O2, LV_S3, LV_O3, LV_S4, LV_O4, LV_5
};

constexpr i64 S_5    = 7'777'777'777;   // 五连
constexpr i64 S_O4   =    77'777'777;   // 活四
constexpr i64 S_S4S4 =    77'777'777;   // 双眠四
constexpr i64 S_S4O3 =    37'777'777;   // 眠四活三
constexpr i64 S_O3O3 =    37'777'777;   // 双活三
constexpr i64 S_S4S3 =       397'777;   // 眠四眠三
constexpr i64 S_S42  =       237'777;   // 眠四二
constexpr i64 S_S4   =       157'777;   // 冲(眠)四
constexpr i64 S_O3S3 =        37'777;   // 活三眠三
constexpr i64 S_O32  =        22'777;   // 活三二
constexpr i64 S_O3   =        15'777;   // 活三
constexpr i64 S_S3S3 =         3'777;   // 双眠三
constexpr i64 S_S3O2 =         2'277;   // 眠三活二
constexpr i64 S_O2O2 =         1'577;   // 双活二
constexpr i64 S_O2S2 =         1'177;   // 活二眠二
constexpr i64 S_S3   =           777;   // 眠三
constexpr i64 S_O2   =           117;   // 活二
constexpr i64 S_S2S2 =            37;   // 双眠二
constexpr i64 S_S2   =             7;   // 眠二


// 0 空 , 1 我方 , 2 敌方或越界
template <cui16 SIZE>
inline ui8 cell_at(ui8 (&board)[SIZE][SIZE], i16 x, i16 y, ui8 me)
{
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) return 2;
    return board[y][x] == 0 ? 0 : (board[y][x] == me ? 1 : 2);
}


// 判断这条线属于什么型
template <cui16 SIZE>
inline Level line_level(ui8 (&board)[SIZE][SIZE], i16 x, i16 y, i16 dx, i16 dy, ui8 me)
{
    ui8 windows[9] = { 0 };                         // 总共九个格
    ui8 bad = 0, cnt = 0;

    for (i16 p = -4; p <= 4; ++p)
    {
        ui8 c = cell_at<SIZE>(board, x + dx * p, y + dy * p, me);
        windows[p + 4] = c;

        if (p <= 1)
        {
            if (c == 2 && p > -4 && p < 1) { ++bad; }
            if (c == 1) { ++cnt; }
        }
    }

    Level best = LV_NONE;

	for (i16 i = 0; i < 4; ++i)                     // 线性扫描窗口
    {
        if (bad || cnt < 2) { goto Slide; }
		if (cnt == 5 && (windows[i] == 1) ^ (windows[i + 5] == 1)) { return LV_5; }
        if (cnt == 5) { best = std::max(best, LV_S4); goto Slide; }

        if (windows[i] && windows[i + 5]) { goto Slide; } // 自己想为什么跳过

        best = std::max(best,
            static_cast<Level>((((cnt - 2) << 1) | 1) + (!windows[i] && !windows[i + 5])));

    Slide:
		if (i == 3) { break; }
        if (windows[i    ] == 1) { --cnt; }
        if (windows[i + 1] == 2) { --bad; }
        if (windows[i + 5] == 2) { ++bad; }
        if (windows[i + 6] == 1) { ++cnt; }
    }
    return best;
}


// 四个方向组合
inline i64 combo_score(const Level (&L)[4])
{
    i16 n4 = 0, n4o = 0, n3o = 0, n3s = 0, n2o = 0, n2s = 0;

    for (i16 d = 0; d < 4; ++d)
    {
        switch (L[d])
        {
        case LV_5:   return S_5;
        case LV_O4:  ++n4; ++n4o; break;
        case LV_S4:  ++n4;  break;
        case LV_O3:  ++n3o; break;
        case LV_S3:  ++n3s; break;
        case LV_O2:  ++n2o; break;
        case LV_S2:  ++n2s; break;
        default:     break;
        }
    }

    i64 best = 0;
    auto up = [&best](i64 v) { if (v > best) best = v; };

    if (n4o >= 1)             up(S_O4);             // 活四
    if (n4  >= 2)             up(S_S4S4);           // 双眠四
    if (n4  >= 1 && n3o >= 1) up(S_S4O3);           // 眠四活三
    if (n3o >= 2)             up(S_O3O3);           // 双活三
    if (n4  >= 1 && n3s >= 1) up(S_S4S3);           // 眠四眠三
    if (n4  >= 1 && n2o >= 1) up(S_S42);            // 眠四二
    if (n4  >= 1)             up(S_S4);             // 冲四
    if (n3o >= 1 && n3s >= 1) up(S_O3S3);           // 活三眠三
    if (n3o >= 1 && n2o >= 1) up(S_O32);            // 活三二
    if (n3o >= 1)             up(S_O3);             // 活三
    if (n3s >= 2)             up(S_S3S3);           // 双眠三
    if (n3s >= 1 && n2o >= 1) up(S_S3O2);           // 眠三活二
    if (n2o >= 2)             up(S_O2O2);           // 双活二
    if (n2o >= 1 && n2s >= 1) up(S_O2S2);           // 活二眠二
    if (n3s >= 1)             up(S_S3);             // 眠三
    if (n2o >= 1)             up(S_O2);             // 活二
    if (n2s >= 2)             up(S_S2S2);           // 双眠二
    if (n2s >= 1)             up(S_S2);             // 眠二

    return best;
}

constexpr i16 ps_dirs[4][2] = { { 1,0 },{ 0,1 },{ 1,1 },{ 1,-1 } };

// 给空点打分
template <cui16 SIZE>
inline i64 eval_point(ui8 (&board)[SIZE][SIZE], i16 x, i16 y, ui8 color)
{
    if (cell_at<SIZE>(board, x, y, color) != 0) return 0;     // 只给空点打分

    board[y][x] = color;

    Level L[4];
    for (i16 d = 0; d < 4; ++d)
    {
        L[d] = line_level<SIZE>(board, x, y, ps_dirs[d][0], ps_dirs[d][1], color);
    }

    board[y][x] = 0;

    return combo_score(L);
}
