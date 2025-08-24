#include <algorithm>

#include "Random.hpp"
#include "Clock.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <stdexcept>
#include <array>
#include <ranges>
#include <deque>


// TODO: Make it impossible for food to appear inside snake
// TODO:

namespace Grid
{
    constexpr int cellSide{ 40 };
    constexpr int rows{ 15 };
    constexpr int columns{ 20 };
    constexpr int pxWidth{ cellSide * columns };
    constexpr int pxHeight{ cellSide * rows };

    class Vec2
    {
    public:
        constexpr Vec2() = default;

        constexpr Vec2(int x, int y)
            : m_x{ x }, m_y{ y }
        {
        }

        constexpr explicit Vec2(const sf::Vector2f& v)
            : m_x{ static_cast<int>(v.x) / m_basisVectorLength }
            , m_y{ static_cast<int>(v.y) / m_basisVectorLength }
        {
        }

        explicit operator sf::Vector2f() const
        {
            return {
                static_cast<float>(m_x * m_basisVectorLength),
                static_cast<float>(m_y * m_basisVectorLength)
            };
        }

        [[nodiscard]] int getX() const { return m_x; }
        [[nodiscard]] int getY() const { return m_y; }

        friend Vec2 operator+(const Vec2& v1, const Vec2& v2)
        {
            return { v1.getX() + v2.getX(), v1.getY() + v2.getY() };
        }

        friend bool operator==(const Vec2& v1, const Vec2& v2)
        {
            return v1.getX() == v2.getX() && v1.getY() == v2.getY();
        }

    private:
        static constexpr int m_basisVectorLength{ cellSide };
        int                  m_x{};
        int                  m_y{};
    };

    auto window{ sf::RenderWindow(sf::VideoMode({ pxWidth, pxHeight }), "Snake") };

    void drawVerticalLines()
    {
        for (int col{ 1 }; col < columns; ++col)
        {
            const float x{ static_cast<float>(col * cellSide) };
            std::array  line{
                sf::Vertex{ { x, 0 } },
                sf::Vertex{ { x, pxHeight } }
            };
            window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        }
    }

    void drawHorizontalLines()
    {
        for (int row{ 1 }; row < rows; ++row)
        {
            const float y{ static_cast<float>(row * cellSide) };
            std::array  line{
                sf::Vertex{ { 0, y } },
                sf::Vertex{ { pxWidth, y } }
            };
            window.draw(line.data(), line.size(), sf::PrimitiveType::Lines);
        }
    }

    void drawGridLines()
    {
        drawVerticalLines();
        drawHorizontalLines();
    }

    Vec2 getRandomPos()
    {
        return {
            Random::get(0, columns - 1),
            Random::get(0, rows - 1)
        };
    }
}

class Snake
{
public:
    enum MoveDirection
    {
        right, up, left, down,
    };

    explicit Snake(Grid::Vec2 position)
    {
        m_segments.emplace_front(position);
    }

    void drawIn(sf::RenderWindow& window) const
    {
        for (const auto& segment: m_segments)
            segment.drawIn(window);
    }

    // Invariant: can't change move direction to the opposite
    // (right to left, top to bottom, and vice versa)
    void setMoveDirection(MoveDirection dir)
    {
        if (!areOpposite(dir, m_moveDirection))
            m_moveDirection = dir;
    }

    void move()
    {
        grow();
        m_segments.pop_back();
    }

    void grow()
    {
        m_segments.emplace_front(getHeadPosition() + getDelta());
    }

    [[nodiscard]] Grid::Vec2 getHeadPosition() const
    {
        return m_segments.front().getPosition();
    }

    [[nodiscard]] bool collidesItself() const
    {
        // check if head collides any other segment
        return std::ranges::any_of(m_segments.cbegin() + 1, m_segments.cend(),
            [this](const Segment& segment){
                return segment.getPosition() == getHeadPosition();
            });
    }

private:
    class Segment
    {
    public:
        explicit Segment(Grid::Vec2 pos)
        {
            m_shape.setPosition(static_cast<sf::Vector2f>(pos));
            m_shape.setFillColor(sf::Color::Red);
        }

        void drawIn(sf::RenderWindow& window) const { window.draw(m_shape); }

        Grid::Vec2 getPosition() const
        {
            return Grid::Vec2{ m_shape.getPosition() };
        }

    private:
        sf::RectangleShape m_shape{ { Grid::cellSide, Grid::cellSide } };
    };

    MoveDirection       m_moveDirection{ right };
    std::deque<Segment> m_segments{};

    [[nodiscard]] Grid::Vec2 getDelta() const
    {
        switch (m_moveDirection)
        {
        case up:
            return { 0, -1 };
        case down:
            return { 0, 1 };
        case left:
            return { -1, 0 };
        case right:
            return { 1, 0 };
        default:
            throw std::runtime_error("Unknown enumerator of MoveDirection");
        }
    }

    [[nodiscard]]
    static bool areOpposite(MoveDirection dir1, MoveDirection dir2)
    {
        return (dir1 == left && dir2 == right
                || dir1 == right && dir2 == left
                || dir1 == up && dir2 == down
                || dir1 == down && dir2 == up);
    }
};

bool collidesWall(const Snake& snake)
{
    const Grid::Vec2 pos{ snake.getHeadPosition() };
    return (pos.getX() < 0
            || pos.getX() > Grid::columns - 1
            || pos.getY() < 0
            || pos.getY() > Grid::rows - 1);
}

class Food
{
public:
    explicit Food(Grid::Vec2 position)
    {
        m_shape.setPosition(static_cast<sf::Vector2f>(position));
        m_shape.setFillColor(sf::Color::Yellow);
    }

    [[nodiscard]] Grid::Vec2 getPosition() const
    {
        return Grid::Vec2{ m_shape.getPosition() };
    }

    void drawIn(sf::RenderWindow& window) const
    {
        window.draw(m_shape);
    }

private:
    sf::CircleShape m_shape{ Grid::cellSide / 2.f };
};

bool ateFood(const Snake& snake, const Food& food)
{
    return snake.getHeadPosition() == food.getPosition();
}

int main()
{
    // ================ INITIALIZE ====================
    Clock clock{};
    clock.start();

    constexpr Grid::Vec2 center{
        Grid::columns / 2,
        Grid::rows / 2
    };

    Snake snake{ center };
    Food  food{ Grid::getRandomPos() };
    Clock::Seconds period{ 0.2 };

    bool gameOver{ false };
    // =================================================

    // Game Loop
    while (Grid::window.isOpen())
    {
        while (const std::optional event{ Grid::window.pollEvent() })
        {
            if (event->is<sf::Event::Closed>())
            {
                Grid::window.close();
            } else if (const auto* keyPressed{ event->getIf<sf::Event::KeyPressed>() })
            {
                using enum sf::Keyboard::Scancode;

                switch (keyPressed->scancode)
                {
                case Escape:
                    Grid::window.close();
                    break;
                case W:
                    snake.setMoveDirection(Snake::up);
                    break;
                case A:
                    snake.setMoveDirection(Snake::left);
                    break;
                case S:
                    snake.setMoveDirection(Snake::down);
                    break;
                case D:
                    snake.setMoveDirection(Snake::right);
                    break;
                default: ;
                }
            }
        }

        // update screen each s seconds =============
        if (clock.elapsed() >= period)
        {
            Grid::window.clear(sf::Color::Black);

            if (!gameOver)
            {
                if (ateFood(snake, food))
                {
                    snake.grow();
                    food = Food{ Grid::getRandomPos() };
                    constexpr Clock::Seconds delta{0.01};
                    constexpr Clock::Seconds minPeriod{0.02};
                    period = std::max(period - delta, minPeriod);
                } else
                {
                    snake.move();
                }

                if (collidesWall(snake) || snake.collidesItself())
                {
                    std::cout << "game over\n";
                    gameOver = true;
                }
            }

            food.drawIn(Grid::window);
            snake.drawIn(Grid::window);

            Grid::drawGridLines();

            Grid::window.display();

            clock.reset();
            // ==========================================
        }
    }
}
