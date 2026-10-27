#include <SFML/Graphics.hpp>
#include <iostream>

const int SIZE = 15;
const int CELL = 36;
const int OFFSET = 20;
int board[SIZE][SIZE] = { 0 }; // 0空，1黑，2白
bool blackTurn = true;
bool gameOver = false;

bool checkWin(int x, int y, int color)
{
    int dirs[4][2] = { { 1,0 },{ 0,1 },{ 1,1 },{ 1,-1 } };
    for (auto& d : dirs)
    {
        int cnt = 1;
        int dx = d[0], dy = d[1];
        for (int i = 1; i < 5; i++)
        {
            int nx = x + dx * i;
            int ny = y + dy * i;
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && board[ny][nx] == color)
                cnt++;
            else break;
        }
        for (int i = 1; i < 5; i++)
        {
            int nx = x - dx * i;
            int ny = y - dy * i;
            if (nx >= 0 && nx < SIZE && ny >= 0 && ny < SIZE && board[ny][nx] == color)
                cnt++;
            else break;
        }
        if (cnt >= 5) return true;
    }
    return false;
}

int main()
{
    sf::RenderWindow win(sf::VideoMode({ OFFSET * 2 + CELL * SIZE, OFFSET * 2 + CELL * SIZE }), "Gomoku");

    while (win.isOpen())
    {
        while (auto event = win.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                win.close();
            }

            if (auto* mouse = event->getIf<sf::Event::MouseButtonPressed>())
            {
                if (mouse->button == sf::Mouse::Button::Left && !gameOver)
                {
                    int mx = mouse->position.x;
                    int my = mouse->position.y;
                    int gx = static_cast<int>(round((mx - OFFSET) / static_cast<double>(CELL)));
                    int gy = static_cast<int>(round((my - OFFSET) / static_cast<double>(CELL)));

                    if (gx >= 0 && gx < SIZE && gy >= 0 && gy < SIZE && board[gy][gx] == 0)
                    {
                        int c = blackTurn ? 1 : 2;
                        board[gy][gx] = c;
                        if (checkWin(gx, gy, c))
                        {
                            gameOver = true;
                        }
                        blackTurn = !blackTurn;
                    }
                }
            }
        }

        win.clear(sf::Color(220, 180, 120));

        // 棋盘横线竖线
        for (int i = 0;i < SIZE;i++)
        {
            sf::Vertex hline[] = {
                sf::Vertex(sf::Vector2f(OFFSET, OFFSET + static_cast<float>(i * CELL)), sf::Color::Black),
                sf::Vertex(sf::Vector2f(OFFSET + static_cast<float>((SIZE - 1) * CELL), OFFSET + static_cast<float>(i * CELL)), sf::Color::Black)
            };
            win.draw(hline, 2, sf::PrimitiveType::Lines);

            sf::Vertex vline[] = {
                sf::Vertex(sf::Vector2f(OFFSET + static_cast<float>(i * CELL), OFFSET), sf::Color::Black),
                sf::Vertex(sf::Vector2f(OFFSET + static_cast<float>(i * CELL), OFFSET + static_cast<float>((SIZE - 1) * CELL)), sf::Color::Black)
            };
            win.draw(vline, 2, sf::PrimitiveType::Lines);
        }

        // 绘制棋子
        for (int y = 0;y < SIZE;y++)
        {
            for (int x = 0;x < SIZE;x++)
            {
                if (board[y][x] == 0) continue;
                sf::CircleShape piece(static_cast<float>(CELL) / 2.0f - 2.0f);
                piece.setPosition(sf::Vector2f(
                    static_cast<float>(OFFSET + x * CELL) - piece.getRadius(),
                    static_cast<float>(OFFSET + y * CELL) - piece.getRadius()
                ));
                if (board[y][x] == 1)
                    piece.setFillColor(sf::Color::Black);
                else
                    piece.setFillColor(sf::Color::White);
                win.draw(piece);
            }
        }
        win.display();
    }
    return 0;
}
