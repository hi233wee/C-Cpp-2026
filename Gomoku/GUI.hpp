#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#include "TEXT.hpp"

typedef unsigned char ui8;
typedef unsigned short int ui16;
typedef short int i16;
typedef const short int ci16;
typedef const unsigned char cui8;
typedef const unsigned short int cui16;

inline constexpr sf::Color board_color = sf::Color(220, 180, 120);
inline constexpr sf::Color bar_color   = sf::Color(198, 160, 104);
inline constexpr sf::Color ink         = sf::Color( 60,  40,  20);

inline constexpr cui16 BAR_W = 77; // 右侧功能键栏宽度, 大概不改了

// 获取窗口
template <cui16 SIZE, cui16 CELL, cui16 OFFSET>
inline sf::RenderWindow get_window()
{
    cui16 side = OFFSET * 2 + CELL * (SIZE - 1);

    return sf::RenderWindow(sf::VideoMode(sf::Vector2u(side + BAR_W, side)), "Gomoku");
}

// 棋盘线
template <cui16 SIZE, cui16 CELL, cui16 OFFSET>
inline void draw_line(sf::RenderWindow& window)
{
    for (ui16 i = 0;i < SIZE;i++)
    {
        sf::Vertex hline[] = {
            sf::Vertex(sf::Vector2f(OFFSET                    , OFFSET + i * CELL), sf::Color::Black),
            sf::Vertex(sf::Vector2f(OFFSET + (SIZE - 1) * CELL, OFFSET + i * CELL), sf::Color::Black)
        };
        window.draw(hline, 2, sf::PrimitiveType::Lines);

        sf::Vertex vline[] = {
            sf::Vertex(sf::Vector2f(OFFSET + i * CELL, OFFSET                    ), sf::Color::Black),
            sf::Vertex(sf::Vector2f(OFFSET + i * CELL, OFFSET + (SIZE - 1) * CELL), sf::Color::Black)
        };
        window.draw(vline, 2, sf::PrimitiveType::Lines);
    } // 两点确定一条直线
}

// 棋子
template <cui16 CELL, cui16 OFFSET>
inline void draw_stone(sf::RenderWindow& window, i16 x, i16 y, ui8 color)
{
    static sf::CircleShape piece(CELL / 2 - 2); // 半径

    piece.setPosition(sf::Vector2f(
        OFFSET + x * CELL - piece.getRadius(),
        OFFSET + y * CELL - piece.getRadius()
    ));
    piece.setFillColor(color == 1 ? sf::Color::Black : sf::Color::White);
    window.draw(piece);
}

// 功能区
template <cui16 side>
inline void draw_bar(sf::RenderWindow& window)
{
    static sf::RectangleShape bar(sf::Vector2f(BAR_W, side));
    bar.setPosition(sf::Vector2f(side, 0));
    bar.setFillColor(bar_color);                // 功能键栏底色
    window.draw(bar);
}

// 按钮  对齐网格  返回中心
template <cui16 CELL, cui16 OFFSET, cui16 side>
inline sf::Vector2f draw_button(sf::RenderWindow& window, ui8 idx)
{
    constexpr float W = (BAR_W / CELL - 1) * CELL - 7;
    constexpr float H = CELL - 7;
    constexpr float X = side + (BAR_W - W) / 2;

    float Y = static_cast<float>(OFFSET + (idx << 1) * CELL) - H / 2;

    static sf::RectangleShape button(sf::Vector2f(W, H));
    button.setPosition(sf::Vector2f(X, Y));
    button.setFillColor(sf::Color(236, 210, 166));
    button.setOutlineColor(sf::Color(120, 88, 48));
    button.setOutlineThickness(1);
    window.draw(button);

    return sf::Vector2f(X + W / 2, Y + H / 2);
}

// 重置
template <cui16 CELL, cui16 OFFSET, cui16 side>
inline void reset(sf::RenderWindow& window)
{
    sf::Vector2f c = draw_button<CELL, OFFSET, side>(window, 1);

    constexpr float PI    = 3.14159265f;
    constexpr float R_OUT = 8.f;        // 外半径
    constexpr float R_IN  = 5.f;        // 内半径
    constexpr float A0    = 300.f;
    constexpr float A1    = 600.f;

    sf::VertexArray arc(sf::PrimitiveType::TriangleStrip);  // 多个矩形模拟扇形
    for (float a = A0; a <= A1; a += 6.f)
    {
        float rad = a * PI / 180.f;
        float cs = std::cos(rad), sn = std::sin(rad);
        arc.append(sf::Vertex(sf::Vector2f(c.x + R_OUT * cs, c.y + R_OUT * sn), ink));
        arc.append(sf::Vertex(sf::Vector2f(c.x + R_IN  * cs, c.y + R_IN  * sn), ink));
    }
    window.draw(arc);

    float rad = A1 * PI / 180.f;
    sf::Vector2f out(std::cos(rad), std::sin(rad));         // 径向
    sf::Vector2f tan(-std::sin(rad), std::cos(rad));        // 顺时针切线
    sf::Vector2f mid = c + out * ((R_OUT + R_IN) / 2.f);

    sf::ConvexShape head(3);
    head.setPoint(0, mid + out * 3.f);
    head.setPoint(1, mid - out * 3.f);
    head.setPoint(2, mid + tan * 6.f);
    head.setFillColor(ink);
    window.draw(head);
}

// 撤销
template <cui16 CELL, cui16 OFFSET, cui16 side>
inline void undo(sf::RenderWindow& window)
{
    sf::Vector2f c = draw_button<CELL, OFFSET, side>(window, 2);

    sf::ConvexShape tri(3);
    tri.setPoint(0, sf::Vector2f(0.f, 0.f));
    tri.setPoint(1, sf::Vector2f(0.f, 12.f));
    tri.setPoint(2, sf::Vector2f(-9.f, 6.f));
    tri.setPosition(sf::Vector2f(c.x + 4.f, c.y - 6.f));
    tri.setFillColor(ink);
    window.draw(tri);
}

// AI
template <cui16 CELL, cui16 OFFSET, cui16 side>
inline void AI(sf::RenderWindow& window)
{
    sf::Vector2f c = draw_button<CELL, OFFSET, side>(window, 3);

    sf::Text text(FONT, "AI", 14);
    text.setFillColor(ink);
    sf::FloatRect box = text.getLocalBounds();
    text.setPosition(sf::Vector2f(
        c.x - box.size.x / 2 - box.position.x,
        c.y - box.size.y / 2 - box.position.y));
    window.draw(text);
}


// 整个屏幕
template <cui16 SIZE, cui16 CELL, cui16 OFFSET>
inline void draw_screen(sf::RenderWindow& window, cui8 (&board)[SIZE][SIZE])
{
    window.clear(board_color);

    draw_line<SIZE, CELL, OFFSET>(window);

    constexpr cui16 side = OFFSET * 2 + CELL * (SIZE - 1);

	draw_bar<side>(window);

    reset<CELL, OFFSET, side>(window);

    undo<CELL, OFFSET, side>(window);

    AI<CELL, OFFSET, side>(window);

    for (ui16 i = 0; i < SIZE; ++i)
    {
        for (ui16 j = 0; j < SIZE; ++j)
        {
            if (board[j][i])
            {
                draw_stone<CELL, OFFSET>(window, i, j, board[j][i]);
            }
        }
    }
}