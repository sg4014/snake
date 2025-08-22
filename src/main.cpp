#include "Clock.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <array>

namespace Grid
{
    constexpr int cellSide{ 40 };
    constexpr int rows{ 15 };
    constexpr int columns{ 20 };
    constexpr int width{ cellSide * columns };
    constexpr int height{ cellSide * rows };

    auto window{ sf::RenderWindow(sf::VideoMode({ width, height }), "Snake") };

    void drawGridLines()
    {
        // draw vertical lines
        for (int col{ 1 }; col < columns; ++col)
        {
            const float x{ static_cast<float>(col * cellSide) };
            std::array  line{
                sf::Vertex{ { x, 0 } },
                sf::Vertex{ { x, height } }
            };
            window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        }

        // draw horizontal lines
        for (int row{ 1 }; row < rows; ++row)
        {
            const float y{ static_cast<float>(row * cellSide) };
            std::array  line{
                sf::Vertex{ { 0, y } },
                sf::Vertex{ { width, y } }
            };
            window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        }
    }
}

class Snake
{
public:
    enum MoveDirection
    {
        right, top, left, bottom,
    };

    explicit Snake(sf::Vector2f position)
    {
        m_shape.setFillColor(sf::Color::Red);
        m_shape.setPosition(position);
    }

    void drawIn(sf::RenderWindow& window) const
    {
        window.draw(m_shape);
    }

    void setMoveDirection(MoveDirection dir) { m_moveDirection = dir; }

    void move()
    {
        sf::Vector2f delta{};

        switch (m_moveDirection)
        {
        case top:
            delta = { 0, -Grid::cellSide };
            break;
        case bottom:
            delta = { 0, Grid::cellSide };
            break;
        case left:
            delta = { -Grid::cellSide, 0 };
            break;
        case right:
            delta = { Grid::cellSide, 0 };
            break;
        default:
            throw std::runtime_error("Unknown enumerator of MoveDirection");
        }

        m_shape.setPosition(m_shape.getPosition() + delta);
    }

private:
    MoveDirection      m_moveDirection{ right };
    sf::RectangleShape m_shape{ { Grid::cellSide, Grid::cellSide } };
};

int main()
{
    // Clock
    Clock clock{};
    clock.start();

    constexpr sf::Vector2f center{
        Grid::cellSide * (Grid::columns / 2),
        Grid::cellSide * (Grid::rows / 2)
    };

    Snake snake{ center };

    // Game Loop
    while (Grid::window.isOpen())
    {
        while (const std::optional event{ Grid::window.pollEvent() })
        {
            if (event->is<sf::Event::Closed>())
            {
                Grid::window.close();
            }
            else if (const auto* keyPressed{ event->getIf<sf::Event::KeyPressed>() })
            {
                using enum sf::Keyboard::Scancode;

                switch (keyPressed->scancode)
                {
                case Escape:
                    Grid::window.close();
                    break;
                case W:
                    snake.setMoveDirection(Snake::top);
                    break;
                case A:
                    snake.setMoveDirection(Snake::left);
                    break;
                case S:
                    snake.setMoveDirection(Snake::bottom);
                    break;
                case D:
                    snake.setMoveDirection(Snake::right);
                    break;
                default: ;
                }
            }
        }

        // update screen each s seconds =============
        Clock::Seconds period {0.2};

        if (clock.elapsed() < period)
            continue;

        Grid::window.clear(sf::Color::Black);

        snake.move();
        snake.drawIn(Grid::window);

        Grid::drawGridLines();

        Grid::window.display();

        clock.reset();
        // ==========================================
    }
}
