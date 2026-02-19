#include <gtest/gtest.h>
#include "game/moving_entity.h"
#include <cmath>

// Concrete implementation because MovingEntity is abstract
class TestMovingEntity : public MovingEntity {
public:
    TestMovingEntity() : MovingEntity() {}

    TestMovingEntity(Vector pos, double radius, Vector velocity, Vector heading, double max_speed, double max_force, double max_turn) 
        : MovingEntity(pos, radius, velocity, heading, max_speed, max_force, max_turn) {}

    void Update() override {
        // Simple integration for testing
        m_Pos += m_Velocity;
    }

    void HandleMessage(const Message& message) override {}
};

TEST(MovingEntityTest, DefaultConstructor) {
    TestMovingEntity e;
    EXPECT_DOUBLE_EQ(e.MaxSpeed(), 0.0);
    EXPECT_TRUE(e.Velocity().isZero());
}

TEST(MovingEntityTest, ParameterizedConstructor) {
    Vector pos(1, 2);
    double radius = 10;
    Vector velocity(5, 0); // Speed 5
    Vector heading(1, 0);
    double maxSpeed = 20;
    double maxForce = 50;
    double maxTurn = 3.14;

    TestMovingEntity e(pos, radius, velocity, heading, maxSpeed, maxForce, maxTurn);

    EXPECT_DOUBLE_EQ(e.MaxSpeed(), 20.0);
    EXPECT_DOUBLE_EQ(e.Velocity().Length(), 5.0);
    EXPECT_DOUBLE_EQ(e.Heading().x, 1.0);
    EXPECT_DOUBLE_EQ(e.Heading().y, 0.0);
}

TEST(MovingEntityTest, SetVelocityTruncatesSpeed) {
    Vector velocity(100, 0);
    double maxSpeed = 10;

    // Use parameterized constructor
    TestMovingEntity e2(Vector(), 0, Vector(), Vector(0,1), maxSpeed, 0, 0);
    e2.SetVelocity(velocity);
    
    EXPECT_DOUBLE_EQ(e2.Speed(), maxSpeed);
    EXPECT_DOUBLE_EQ(e2.Velocity().Length(), 10.0);
}

TEST(MovingEntityTest, HeadingAndSide) {
    Vector heading(1, 0);
    TestMovingEntity e(Vector(), 0, Vector(), heading, 0, 0, 0);

    // Initial heading (1,0) -> Side is perp: (0, 1) or (0, -1) depending on implementation
    EXPECT_DOUBLE_EQ(e.Side().x, 0.0);
    EXPECT_DOUBLE_EQ(e.Side().y, 1.0);

    // Set new heading
    Vector newHeading(0, 1);
    e.SetHeading(newHeading);
    EXPECT_DOUBLE_EQ(e.Heading().x, 0.0);
    EXPECT_DOUBLE_EQ(e.Heading().y, 1.0);
    // Side: (-1, 0)
    EXPECT_DOUBLE_EQ(e.Side().x, -1.0);
    EXPECT_DOUBLE_EQ(e.Side().y, 0.0);
}
