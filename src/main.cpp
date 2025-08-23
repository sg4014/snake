#include "Clock.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <array>
#include <utility>

// TODO: spawn a yellow circle in some cell
// TODO: make the snake grow when it eats a yellow circle
// TODO: make the snake move faster when it eats a yellow circle

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
        m_shape.setPosition(m_shape.getPosition() + getDelta());
    }

    void grow()
    {

    }

    [[nodiscard]] sf::Vector2f getPosition() const
    {
        return m_shape.getPosition();
    }

private:
    MoveDirection      m_moveDirection{ right };
    sf::RectangleShape m_shape{ { Grid::cellSide, Grid::cellSide } };

    sf::Vector2f getDelta() const
    {
        switch (m_moveDirection)
        {
        case top:    return { 0, -Grid::cellSide };
        case bottom: return { 0, Grid::cellSide };
        case left:   return { -Grid::cellSide, 0 };
        case right:  return { Grid::cellSide, 0 };
        default:
            throw std::runtime_error("Unknown enumerator of MoveDirection");
        }
    }
};

bool collidesWall(const Snake& snake)
{
    const sf::Vector2f pos{ snake.getPosition() };
    return (pos.x < 0
            || pos.x > Grid::width - Grid::cellSide
            || pos.y < 0
            || pos.y > Grid::height - Grid::cellSide);
}

class Food
{
public:
    explicit Food(sf::Vector2f position)
    {
        m_shape.setPosition(position);
        m_shape.setFillColor(sf::Color::Yellow);
    }

    void drawIn(sf::RenderWindow& window) const
    {
        window.draw(m_shape);
    }

private:
    sf::CircleShape m_shape{ Grid::cellSide / 2 };
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
    Food  food{ center };

    bool gameOver{ false };

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
        Clock::Seconds period{ 0.2 };

        if (clock.elapsed() < period)
            continue;

        Grid::window.clear(sf::Color::Black);

        if (!gameOver)
            snake.move();

        if (!gameOver && collidesWall(snake))
        {
            std::cout << "game over\n";
            gameOver = true;
        }

        food.drawIn(Grid::window);
        snake.drawIn(Grid::window);

        Grid::drawGridLines();

        Grid::window.display();

        clock.reset();
        // ==========================================
    }
}
