#include <iostream>
#include <cstring>

typedef int i32;
typedef short int i16;
typedef const short int ci16;
typedef unsigned char ui8;
typedef unsigned short int ui16;
typedef const unsigned char cui8;
typedef const unsigned short int cui16;

bool check_win(i16 x, i16 y, ui8 color); // 有人要用

#include "GUI.hpp"
#include "TEXT.hpp"
#include "AI.hpp"

static constexpr cui16 SIZE = 15;       // SIZE * SIZE
static constexpr cui16 CELL = 18<<1;    // 一个格子的像素
static constexpr cui16 OFFSET = 32;     // 棋盘边上空的像素

static ui8 board[SIZE][SIZE] = { 0 };   // 0 空 , 1 黑 , 2 白
static ui16 step_x[SIZE * SIZE] = { 0 };
static ui16 step_y[SIZE * SIZE] = { 0 };
static ui16 step_count = 0;
static ui8 turn = 1;
static ui8 winner = 0; // 0没结束, 1黑, 2白


int main()
{
    sf::RenderWindow window = get_window< SIZE, CELL, OFFSET>();
	load_font();
   
    while (window.isOpen())
    {
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) { window.close(); }

            if (auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                // 下棋
                if (mouse->button == sf::Mouse::Button::Left)
                {
                    i16 gx = std::round((mouse->position.x - OFFSET) / static_cast<double>(CELL));
                    i16 gy = std::round((mouse->position.y - OFFSET) / static_cast<double>(CELL));

                    if (!winner)
                    {
                        if (gx >= 0 && gx < SIZE && gy >= 0 && gy < SIZE && board[gy][gx] == 0)
                        {
                            board[gy][gx] = turn;
                            step_x[step_count] = gx;
                            step_y[step_count] = gy;
                            ++step_count;
                            if (check_win(gx, gy, turn)) { winner = turn; }
                            turn = turn == 1 ? 2 : 1;
                        }
                    }

                    if (gx == SIZE + 1)
                    {
                        // 重置
                        if (gy == 2)
                        {
                            memset(board, 0, sizeof(board));
                            memset(step_x, 0, sizeof(step_x));
                            memset(step_y, 0, sizeof(step_y));
                            step_count = 0;
                            turn = 1;
                            winner = 0;
                        }

                        // 撤销
						if (gy == 4)
						{
                            if (step_count > 0)
                            {
                                --step_count;
                                board[step_y[step_count]][step_x[step_count]] = 0;
                                turn = turn == 1 ? 2 : 1;
                                winner = 0;
                            }
						}

                        // AI
					    if (gy == 6 && !winner)
						{
                            sf::Vector2u ai_move = AI<SIZE>(board, turn);
                            board[ai_move.y][ai_move.x] = turn;
                            step_x[step_count] = ai_move.x;
                            step_y[step_count] = ai_move.y;
                            ++step_count;
                            if (check_win(ai_move.x, ai_move.y, turn)) { winner = turn; }
                            turn = turn == 1 ? 2 : 1;
                        }
                    }

					if (step_count >= SIZE * SIZE) { winner = 3; } // 平局
                }
            }
        }

        draw_screen<SIZE, CELL, OFFSET>(window, board);

        auto mousePos = sf::Mouse::getPosition(window);
        i16 gx = std::round((mousePos.x - OFFSET) / static_cast<double>(CELL));
        i16 gy = std::round((mousePos.y - OFFSET) / static_cast<double>(CELL));
        if (!winner && gx >= 0 && gx < SIZE && gy >= 0 && gy < SIZE && board[gy][gx] == 0)
        {
            draw_stone<CELL, OFFSET>(window, gx, gy, turn);
        }

        if (winner)
        {
            draw_text(window, winner == 3 ? "Draw!" : winner == 1 ? "Black wins!" : "White wins!");
        }

        window.display();
    }

    return 0;
}


constexpr i16 dirs[4][2] = { { 1,0 },{ 0,1 },{ 1,1 },{ 1,-1 } };
bool check_win(i16 x, i16 y, ui8 color)
{
    for (auto& d : dirs)
    {
        ui8 cnt = 1, nx, ny;
        i16 dx = d[0], dy = d[1];
        for (ui8 i = 1; i < 5; ++i)
        {
            nx = x + dx * i;
            ny = y + dy * i;
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && board[ny][nx] == color)
            {
                cnt++;
            }
            else { break; }
        }
        for (ui8 i = 1; i < 5; ++i)
        {
            nx = x - dx * i;
            ny = y - dy * i;
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && board[ny][nx] == color)
            {
                cnt++;
            }
            else { break; }
        }

        if (cnt >= 5) { return true; }
    }
    return false;
}
