#ifndef SNAKE_RANDOM_HPP
#define SNAKE_RANDOM_HPP

#include <chrono>
#include <random>

namespace Random
{
    inline std::mt19937 getSeededGenerator()
    {
        std::random_device rd{};

        std::seed_seq ss{
            static_cast<std::seed_seq::result_type>(
                std::chrono::steady_clock::now().time_since_epoch().count()
            ),
            rd(), rd(), rd(), rd(), rd(), rd(), rd()
        };

        return std::mt19937{ ss };
    }

    inline std::mt19937 mt{ getSeededGenerator() };

    // returns a random int between [min, max] (inclusive)
    inline int get(int min, int max)
    {
        return std::uniform_int_distribution{min, max}(mt);
    }
}

#endif //SNAKE_RANDOM_HPP
