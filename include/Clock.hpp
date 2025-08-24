#ifndef SNAKE_CLOCK_HPP
#define SNAKE_CLOCK_HPP

#include <chrono>

class Clock
{
public:
    using Seconds   = std::chrono::duration<float>;
    using TimePoint = std::chrono::time_point<std::chrono::steady_clock, Seconds>;

    [[nodiscard]] static TimePoint now()
    {
        return std::chrono::steady_clock::now();
    }

    void start() { m_start = now(); }

    [[nodiscard]] Seconds elapsed() const
    {
        return now() - m_start;
    }

    Seconds reset()
    {
        Seconds temp {elapsed()};
        start();
        return temp;
    }
private:
    TimePoint m_start{};
};

#endif //SNAKE_CLOCK_HPP