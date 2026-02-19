#pragma once
#include <chrono>

#define Timer TimeSystem::Instance()

class TimeSystem {
    std::chrono::steady_clock::time_point m_Start;

    TimeSystem() : m_Start(std::chrono::steady_clock::now()) {}

    TimeSystem(const TimeSystem&) = delete;
    TimeSystem& operator=(const TimeSystem&) = delete;

public:
    double g_Time = 0.0;
    double g_TimeElapsed = 0.0;
    static TimeSystem* Instance() {
        static TimeSystem TimeSystem;

        return &TimeSystem;
    }

    /// @note called once per frame by the main game loop.
    inline void Update() {
        double time = GetTimeSince(m_Start);
        g_TimeElapsed = time - g_Time;
        g_Time = time;
    }

    double GetTimeSince(const std::chrono::steady_clock::time_point& since) const {
        auto current = std::chrono::steady_clock::now();

        auto elapsed = std::chrono::duration<double>(current - since);

        return elapsed.count();
    }

    inline bool IsPassed(const double time) const {
        return g_Time >= time;
    }
};