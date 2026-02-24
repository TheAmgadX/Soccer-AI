#pragma  once
#include <random>

namespace Utils {
    inline double RandomFloatClamped() {
        static std::mt19937_64 rng(std::random_device{}()); // seeded once
        static std::uniform_real_distribution<double> dist(-1.0, 1.0);

        return dist(rng);
    }

    inline double Range(double min, double max) {
        static std::mt19937_64 rng(std::random_device{}()); // seeded once
        std::uniform_real_distribution<double> dist(min, max);

        return dist(rng);
    }

    inline int RandomInt(int min, int max) {
        static std::mt19937_64 rng(std::random_device{}()); // seeded once
        std::uniform_int_distribution<int> dist(min, max);

        return dist(rng);
    }
}
