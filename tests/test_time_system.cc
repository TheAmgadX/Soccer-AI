#include <gtest/gtest.h>
#include "utils/time_system.h"
#include <thread>

TEST(TimeSystemTest, Singleton) {
    TimeSystem* t1 = TimeSystem::Instance();
    TimeSystem* t2 = TimeSystem::Instance();
    EXPECT_EQ(t1, t2);
}

TEST(TimeSystemTest, UpdateIncreasesTime) {
    TimeSystem* t = TimeSystem::Instance();
    
    // Initial update
    t->Update();
    double initialTime = t->g_Time;
    
    // Wait a bit
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // Second update
    t->Update();
    double newTime = t->g_Time;
    
    EXPECT_GT(newTime, initialTime);
    EXPECT_GT(t->g_TimeElapsed, 0.0);
}

TEST(TimeSystemTest, ManualTimeControl) {
    TimeSystem* t = TimeSystem::Instance();
    
    // We can manually set time for deterministic testing of other systems
    t->g_Time = 100.0;
    EXPECT_TRUE(t->IsPassed(99.0));
    EXPECT_FALSE(t->IsPassed(101.0));
}
