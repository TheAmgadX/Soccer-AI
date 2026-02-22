#pragma  once
#include <random>

namespace Utils {
    inline double RandomFloatClamped() {
        static std::mt19937_64 rng(std::random_device{}()); // seeded once
        static std::uniform_real_distribution<double> dist(-1.0, 1.0);

        return dist(rng);
    }
}
