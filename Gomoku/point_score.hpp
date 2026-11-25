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
    LV_NONE = 0,
    LV_JJO2, LV_JO2, LV_CO2,         // 跳两格 / 跳一格 / 连
    LV_S3,                           // 眠三 (不分跳连)
    LV_JO3, LV_CO3,                  // 跳活三 / 连活三
    LV_S4,  LV_O4,  LV_5
};

constexpr i64 S_5    = 77'777'777'777;   // 五连       // 直接赢
constexpr i64 S_O4   =  7'777'777'777;   // 活四       // 一强
constexpr i64 S_S4S4 =  7'777'777'777;   // 双眠四     // 一强
constexpr i64 S_S4O3 =     77'777'777;   // 眠四活三   // 先中后强
constexpr i64 S_O3O3 =     17'777'777;   // 双活三     // 先弱后强
constexpr i64 S_S4S3 =      3'777'777;   // 眠四眠三   // 先中后中
constexpr i64 S_S4O2 =      1'777'777;   // 眠四活二   // 先中后弱
constexpr i64 S_S4   =        777'777;   // 冲(眠)四   // 一中
constexpr i64 S_O3S3 =         77'777;   // 活三眠三   // 先弱后中
constexpr i64 S_O3O2 =         37'777;   // 活三活二   // 先弱后弱
constexpr i64 S_CO3  =          7'777;   // 连活三     // 一弱
constexpr i64 S_JO3  =          6'777;   // 跳活三     // 一弱
constexpr i64 S_S3S3 =          3'777;   // 双眠三     // 后置两中
constexpr i64 S_S3O2 =          1'777;   // 眠三活二   // 后置一中一弱
constexpr i64 S_S3   =            377;   // 眠三       // 后置一中攻
constexpr i64 S_O2O2 =            177;   // 双活二     // 后置两弱
constexpr i64 S_CO2  =             37;   // 连活二     // 后置一弱攻
constexpr i64 S_JO2  =             17;   // 跳一格的活二
constexpr i64 S_JJO2 =              7;   // 跳两格的活二
// 强不可打断, 先中可被 中+堵 打断, 弱可被中打断


inline Level* g_levels[4] = { nullptr };

template <cui16 SIZE>
inline void init_levels()
{
    for (ui8 i = 0; i < 4; ++i) { g_levels[i] = new Level[SIZE * SIZE](); }
}

// 0 空 , 1 我方 , 2 敌方或越界
template <cui16 SIZE>
inline ui8 cell_at(ui8 (&board)[SIZE][SIZE], i16 x, i16 y, ui8 me)
{
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE) return 2;
    return board[y][x] == 0 ? 0 : (board[y][x] == me ? 1 : 2);
}


inline Level enlevel(ui8* window, i16 bad, i16 cnt)
{
    if (bad || cnt < 2) { return LV_NONE; }
    if (cnt == 5 && (window[0] == 1) ^ (window[5] == 1)) { return LV_5; }
    if (cnt == 5) { return LV_S4; }

    if (window[0] || window[5])
    {
        if (window[0] == window[5]) { return LV_NONE; }
        // 两头堵是死
        // 两头都是自己的
        // 不是空得太多
        // 就是是四, 在其他窗口有判断
        else if (cnt == 4) { return LV_S4; }
        else if (cnt == 3) { return LV_S3; }
        // 一端有自己的也算
    }
    else
    {
        if (cnt == 4) { return LV_O4; }
        else if (cnt == 3) {
            return (window[1] == 1) && (window[4] == 1) ? LV_JO3 : LV_CO3;
        }
        else if (cnt == 2)
        {
                 if (window[1] == 1 && window[4] == 1) { return LV_JJO2; }
            else if (window[1] == 1 && window[2] == 1) { return LV_CO2; }
            else if (window[3] == 1 && window[4] == 1) { return LV_CO2; }
            else { return LV_JO2; }
        }
    }
    return LV_NONE;
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
        best = std::max(best, enlevel(windows + i, bad, cnt));
        if (best == LV_5) { return LV_5; }

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
    i16 n4s = 0, n4o = 0, n3o = 0, n3c = 0, n3s = 0, n2o = 0, n2c = 0, n2j = 0;

    for (i16 d = 0; d < 4; ++d)
    {
        switch (L[d])
        {
        case LV_5:    return S_5;
        case LV_O4:   ++n4s; ++n4o; break;
        case LV_S4:   ++n4s; break;
        case LV_CO3:  ++n3o; ++n3c; break;
        case LV_JO3:  ++n3o; break;
        case LV_S3:   ++n3s; break;
        case LV_CO2:  ++n2o; ++n2c; break;
        case LV_JO2:  ++n2o; ++n2j; break;
        case LV_JJO2: ++n2o; break;
        default:     break;
        }
    }

    i64 best = 0;
    auto up = [&best](i64 v) { if (v > best) best = v; };

    if (n4o >= 1)             up(S_O4  );           // 活四
    if (n4s >= 2)             up(S_S4S4);           // 双眠四
    if (n4s >= 1 && n3o >= 1) up(S_S4O3);           // 眠四活三
    if (n3o >= 2)             up(S_O3O3);           // 双活三
    if (n4s >= 1 && n3s >= 1) up(S_S4S3);           // 眠四眠三
    if (n4s >= 1 && n2o >= 1) up(S_S4O2);           // 眠四活二
    if (n4s >= 1)             up(S_S4  );           // 冲(眠)四
    if (n3o >= 1 && n3s >= 1) up(S_O3S3);           // 活三眠三
    if (n3o >= 1 && n2o >= 1) up(S_O3O2);           // 活三活二
	if (n3c >= 1)             up(S_CO3 );           // 连活三
    if (n3o >= 1)             up(S_JO3 );           // 跳活三
    if (n3s >= 2)             up(S_S3S3);           // 双眠三
    if (n3s >= 1 && n2o >= 1) up(S_S3O2);           // 眠三活二
    if (n2o >= 2)             up(S_O2O2);           // 双活二
    if (n3s >= 1)             up(S_S3  );           // 眠三
	if (n2c >= 1)             up(S_CO2 );           // 连活二
    if (n2j >= 1)             up(S_JO2 );           // 跳一格的活二
    if (n2o >= 1)             up(S_JJO2);           // 跳两格的活二

    return best;
}


constexpr i16 ps_dirs[4][2] = { { 1,0 },{ 0,1 },{ 1,1 },{ 1,-1 } };
// 给空点打分
template <cui16 SIZE>
inline i64 eval_point(ui8 (&board)[SIZE][SIZE], i16 x, i16 y, ui8 me)
{
    if (cell_at<SIZE>(board, x, y, me) == 2) return 0;
	bool has_stone = board[y][x];

    if (!has_stone) { board[y][x] = me; }

    Level L[4] = {};

    for (i16 d = 0; d < 4; ++d)
    {
        L[d] = line_level<SIZE>(board, x, y, ps_dirs[d][0], ps_dirs[d][1], me);
        if (has_stone) { g_levels[d][x + y * SIZE] = L[d]; }
    }

    if (!has_stone) { board[y][x] = 0; }

    return combo_score(L);
}
