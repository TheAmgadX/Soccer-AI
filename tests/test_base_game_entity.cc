#include <gtest/gtest.h>
#include "game/base_game_entity.h"
#include <iostream>

// Minimal concrete implementation for testing the abstract base class
class TestEntity : public Entity {
public:
    TestEntity() : Entity() {}
    TestEntity(Vector pos, double radius) : Entity(pos, radius) {}

    void Update() override {
        // No-op for base tests
    }

    void HandleMessage(const Message& message) override {
        // No-op for base tests
    }
};

TEST(BaseGameEntityTest, DefaultConstructor) {
    TestEntity entity;
    // We can't predict the exact ID, but checking it's non-zero is good practice
    EXPECT_GT(entity.Id(), 0);
    EXPECT_DOUBLE_EQ(entity.Pos().x, 0.0);
    EXPECT_DOUBLE_EQ(entity.Pos().y, 0.0);
    EXPECT_DOUBLE_EQ(entity.Radius(), 0.0);
    EXPECT_FALSE(entity.GetTag());
}

TEST(BaseGameEntityTest, ParameterizedConstructor) {
    Vector initialPos(10.5, -5.2);
    double initialRadius = 3.14;
    TestEntity entity(initialPos, initialRadius);

    EXPECT_GT(entity.Id(), 0);
    EXPECT_DOUBLE_EQ(entity.Pos().x, 10.5);
    EXPECT_DOUBLE_EQ(entity.Pos().y, -5.2);
    EXPECT_DOUBLE_EQ(entity.Radius(), 3.14);
    EXPECT_FALSE(entity.GetTag());
}

TEST(BaseGameEntityTest, UniqueIDs) {
    TestEntity e1;
    TestEntity e2;
    EXPECT_NE(e1.Id(), e2.Id());
}

TEST(BaseGameEntityTest, Tagging) {
    TestEntity entity;
    EXPECT_FALSE(entity.GetTag());

    entity.Tag();
    EXPECT_TRUE(entity.GetTag());

    entity.UnTag();
    EXPECT_FALSE(entity.GetTag());
}
