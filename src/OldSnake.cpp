#include "Clock.hpp"
#include <SFML/Graphics.hpp>

class OldSnake
{
public:
    enum Direction
    {
        top, bottom, left, right,
    };

    explicit OldSnake(float speed = 50, sf::Vector2f position = { 0, 0 })
        : m_velocity{ sf::Vector2f{ 0.f, -speed } }
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
            m_velocity  = getUpdatedVelocity();
        }
    }

private:
    sf::RectangleShape m_shape{ { 40, 40 } };
    Direction          m_direction{};
    sf::Vector2f       m_velocity{};

    sf::Vector2f getUpdatedVelocity() const
    {
        switch (m_direction)
        {
        case top: return { 0.f, -m_velocity.length() };
        case bottom: return { 0.f, m_velocity.length() };
        case left: return { -m_velocity.length(), 0.f };
        case right: return { m_velocity.length(), 0.f };
        default:
            throw std::out_of_range(
                "Impossible enumerator encountered."
                " Only 'top', 'bottom', 'left', 'right' are possible."
            );
        }
    }
};