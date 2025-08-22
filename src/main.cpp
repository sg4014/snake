#include "Clock.hpp"
#include <SFML/Graphics.hpp>
#include <chrono>
#include <iostream>
#include <stdexcept>

class Snake
{
public:
    enum Direction
    {
        top, bottom, left, right,
    };

    explicit Snake(float speed, sf::Vector2f position = { 0, 0 })
        : m_speed{ speed }
    {
        m_shape.setOrigin(m_shape.getGeometricCenter());
        m_shape.setPosition(position);
        m_shape.setFillColor(sf::Color::Red);
    }

    void move(Clock::Seconds deltaTime)
    {
        m_shape.setPosition(m_shape.getPosition() + deltaTime.count() * m_velocity);
    }

    sf::Vector2f getPosition() const { return m_shape.getPosition(); }

    void drawIn(sf::RenderWindow& window) const { window.draw(m_shape); }

    void setDirection(Direction direction)
    {
        if (direction != m_direction)
        {
            m_direction = direction;
            m_velocity = getUpdatedVelocity(direction);
        }
    }

private:
    sf::RectangleShape m_shape{ { 40, 40 } };
    Direction          m_direction{};
    float              m_speed{};
    sf::Vector2f       m_velocity{};

    sf::Vector2f getUpdatedVelocity(Direction direction) const
    {
        switch (direction)
        {
        case top:    return { 0.f, m_speed };
        case bottom: return { 0.f, -m_speed };
        case left:   return { -m_speed, 0.f };
        case right:  return { m_speed, 0.f };
        default:
            throw std::out_of_range(
                "Impossible enumerator encountered."
                " Only 'top', 'bottom', 'left', 'right' are possible."
            );
        }
    }
};

void update(Clock::Seconds dt, Snake& snake);

int main()
{
    constexpr int WIDTH{ 600 };
    constexpr int HEIGHT{ 400 };

    auto window = sf::RenderWindow(sf::VideoMode({ WIDTH, HEIGHT }), "Snake");

    // Snake
    constexpr float speed{ 50.f }; // pixels per second
    Snake           snake{ speed, { WIDTH / 2.f, HEIGHT / 2.f } };

    // Clock
    Clock clock{};
    clock.start();

    // Game Loop
    while (window.isOpen())
    {
        while (const std::optional event{ window.pollEvent() })
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto* keyPressed{ event->getIf<sf::Event::KeyPressed>() })
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
            }
        }

        // Clear and Update
        window.clear(sf::Color::Black);

        update(clock.reset(), snake);

        // Draw and display
        snake.drawIn(window);
        window.display();
    }
}

void update(Clock::Seconds dt, Snake& snake)
{
    snake.move(dt);
    // square.setPosition(square.getPosition() + dt.count() * velocity);
}
