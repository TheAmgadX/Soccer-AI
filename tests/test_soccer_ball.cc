#include "game/entities/soccer_ball.h"
#include "math/vector.h"
#include "utils/constants.h"
#include "utils/time_system.h"
#include "utils/wall.h"
#include <gtest/gtest.h>
#include <vector>

class SoccerBallTest : public ::testing::Test {
protected:
  std::vector<Wall> walls;
  TimeSystem *timer;
  Vector startPos;
  SoccerBall *ball;

  void SetUp() override {
    startPos = Vector(100, 100);
    timer = TimeSystem::Instance();
    // Create some walls for collision testing
    walls.push_back(Wall(Vector(0, 0), Vector(200, 0)));     // Top
    walls.push_back(Wall(Vector(200, 0), Vector(200, 200))); // Right
    walls.push_back(Wall(Vector(200, 200), Vector(0, 200))); // Bottom
    walls.push_back(Wall(Vector(0, 200), Vector(0, 0)));     // Left

    ball = new SoccerBall(startPos, 5.0, walls, timer);
  }

  void TearDown() override { delete ball; }
};

TEST_F(SoccerBallTest, InitialState) {
  EXPECT_DOUBLE_EQ(ball->Pos().x, 100.0);
  EXPECT_DOUBLE_EQ(ball->Pos().y, 100.0);
  EXPECT_TRUE(ball->Velocity().isZero());
}

TEST_F(SoccerBallTest, Kick) {
  Vector dir(1, 0);
  double force = 10.0;
  ball->Kick(dir, force);

  // Velocity should be in the direction of kick (with some noise handled by
  // AddNoiseToKick) Note: AddNoiseToKick might change the direction slightly.
  EXPECT_GT(ball->Velocity().Length(), 0.0);
}

TEST_F(SoccerBallTest, UpdateAndFriction) {
  Vector dir(1, 0);
  double force = 10.0;
  ball->Kick(dir, force);

  Vector initialVel = ball->Velocity();
  timer->g_TimeElapsed = 0.1;
  ball->Update();

  // Velocity should decrease due to friction (FRICTION is negative)
  EXPECT_LT(ball->Velocity().Length(), initialVel.Length());
}

TEST_F(SoccerBallTest, WallCollision) {
  // Place ball so it's touching the top wall (y=0, normal=(0,1))
  // Radius is 5.0, so at y=5.0 it should collide.
  ball->PlaceAtPos(Vector(100, 5.0));
  Vector dir(0, -1); // Upwards
  double force = 10.0;
  ball->Kick(dir, force);

  // Manually force a collision check
  ball->Update();

  // Velocity y should now be positive (reflected)
  EXPECT_GT(ball->Velocity().y, 0.0);
}

TEST_F(SoccerBallTest, FuturePos) {
  ball->PlaceAtPos(Vector(100, 100));
  Vector dir(1, 0);
  ball->Kick(dir, 10.0);

  Vector future = ball->FuturePos(1.0);
  EXPECT_NE(future.x, 100.0);
}

TEST_F(SoccerBallTest, TimeToCoverDistance) {
  Vector from(0, 0);
  Vector to(100, 0);
  double force = 20.0;

  double time = ball->TimeToCoverDistance(from, to, force);
  EXPECT_GT(time, 0.0);

  // If force is too low to reach, it should return -1
  double time2 = ball->TimeToCoverDistance(from, to, 0.1);
  EXPECT_EQ(time2, -1.0);
}

TEST_F(SoccerBallTest, Trap) {
  Vector dir(1, 0);
  ball->Kick(dir, 10.0);
  EXPECT_FALSE(ball->Velocity().isZero());

  ball->Trap();
  EXPECT_TRUE(ball->Velocity().isZero());
}
