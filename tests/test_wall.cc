#include "math/vector.h"
#include "utils/wall.h"
#include <gtest/gtest.h>

TEST(WallTest, Constructor) {
  Vector start(0, 0);
  Vector end(10, 0);
  Wall wall(start, end);

  EXPECT_DOUBLE_EQ(wall.From().x, 0.0);
  EXPECT_DOUBLE_EQ(wall.From().y, 0.0);
  EXPECT_DOUBLE_EQ(wall.To().x, 10.0);
  EXPECT_DOUBLE_EQ(wall.To().y, 0.0);
}

TEST(WallTest, Normal) {
  // Horizontal wall from left to right
  Vector start(0, 0);
  Vector end(10, 0);
  Wall wall(start, end);

  // Normal should be (0, 1) or (0, -1) depending on implementation
  // Implementation:
  // m_Normal.x = -tmp.y;
  // m_Normal.y = tmp.x;
  // tmp = (1, 0) -> Normal = (0, 1)
  EXPECT_DOUBLE_EQ(wall.Normal().x, 0.0);
  EXPECT_DOUBLE_EQ(wall.Normal().y, 1.0);

  // Vertical wall from top to bottom
  Wall wall2(Vector(0, 0), Vector(0, 10));
  // tmp = (0, 1) -> Normal = (-1, 0)
  EXPECT_DOUBLE_EQ(wall2.Normal().x, -1.0);
  EXPECT_DOUBLE_EQ(wall2.Normal().y, 0.0);
}

TEST(WallTest, CreateWalls) {
  std::vector<Wall> &walls = Wall::CreateWalls();
  EXPECT_EQ(walls.size(), 4);

  // Test that they are static/singleton-ish
  std::vector<Wall> &walls2 = Wall::CreateWalls();
  EXPECT_EQ(&walls, &walls2);
}
