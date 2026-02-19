#include <gtest/gtest.h>
#include "math/vector.h"
#include "utils/constants.h"
#include <cmath>

TEST(VectorTest, Constructor) {
    Vector v1;
    EXPECT_DOUBLE_EQ(v1.x, 0.0);
    EXPECT_DOUBLE_EQ(v1.y, 0.0);

    Vector v2(3.5, -2.1);
    EXPECT_DOUBLE_EQ(v2.x, 3.5);
    EXPECT_DOUBLE_EQ(v2.y, -2.1);
}

TEST(VectorTest, Zero) {
    Vector v(10, 20);
    v.Zero();
    EXPECT_DOUBLE_EQ(v.x, 0.0);
    EXPECT_DOUBLE_EQ(v.y, 0.0);
    EXPECT_TRUE(v.isZero());
}

TEST(VectorTest, Length) {
    Vector v(3, 4);
    EXPECT_DOUBLE_EQ(v.LengthSQ(), 25.0);
    EXPECT_DOUBLE_EQ(v.Length(), 5.0);

    Vector zero;
    EXPECT_DOUBLE_EQ(zero.Length(), 0.0);
}

TEST(VectorTest, Normalize) {
    Vector v(10, 0);
    v.Normalize();
    EXPECT_DOUBLE_EQ(v.x, 1.0);
    EXPECT_DOUBLE_EQ(v.y, 0.0);

    Vector v2(3, 4);
    v2.Normalize();
    EXPECT_DOUBLE_EQ(v2.Length(), 1.0);
    EXPECT_DOUBLE_EQ(v2.x, 0.6);
    EXPECT_DOUBLE_EQ(v2.y, 0.8);

    // Test zero vector normalization (should handle gracefully)
    Vector zero;
    zero.Normalize();
    EXPECT_DOUBLE_EQ(zero.x, 0.0);
    EXPECT_DOUBLE_EQ(zero.y, 0.0);
}

TEST(VectorTest, DotProduct) {
    Vector v1(1, 0);
    Vector v2(0, 1);
    EXPECT_DOUBLE_EQ(v1.Dot(v2), 0.0); // Perpendicular

    Vector v3(2, 3);
    Vector v4(4, 5);
    // 2*4 + 3*5 = 8 + 15 = 23
    EXPECT_DOUBLE_EQ(v3.Dot(v4), 23.0);
}

TEST(VectorTest, Operators) {
    Vector v1(1, 2);
    Vector v2(3, 4);

    // Addition
    Vector sum = v1 + v2;
    EXPECT_DOUBLE_EQ(sum.x, 4.0);
    EXPECT_DOUBLE_EQ(sum.y, 6.0);

    // Subtraction
    Vector diff = v1 - v2;
    EXPECT_DOUBLE_EQ(diff.x, -2.0);
    EXPECT_DOUBLE_EQ(diff.y, -2.0);

    // Scalar Multiplication
    Vector scaled = v1 * 2.0;
    EXPECT_DOUBLE_EQ(scaled.x, 2.0);
    EXPECT_DOUBLE_EQ(scaled.y, 4.0);

    // Scalar Division
    Vector divided = v2 / 2.0;
    EXPECT_DOUBLE_EQ(divided.x, 1.5);
    EXPECT_DOUBLE_EQ(divided.y, 2.0);
    
    // Check +=
    v1 += v2;
    EXPECT_DOUBLE_EQ(v1.x, 4.0);
    EXPECT_DOUBLE_EQ(v1.y, 6.0);
}

TEST(VectorTest, Distance) {
    Vector v1(1, 1);
    Vector v2(4, 5);
    // dx = 3, dy = 4, dist = 5
    EXPECT_DOUBLE_EQ(v1.Distance(v2), 5.0);
    EXPECT_DOUBLE_EQ(v1.DistanceSQ(v2), 25.0);
}

TEST(VectorTest, Truncate) {
    Vector v(10, 0);
    v.Truncate(5.0);
    EXPECT_DOUBLE_EQ(v.Length(), 5.0);
    EXPECT_DOUBLE_EQ(v.x, 5.0);

    // Truncate larger than length should do nothing
    v.Truncate(10.0);
    EXPECT_DOUBLE_EQ(v.Length(), 5.0);
}

TEST(VectorTest, Reflection) {
    Vector v(1, 1);
    Vector normal(0, 1); // Wall is horizontal
    
    v.Reflect(normal);
    EXPECT_DOUBLE_EQ(v.x, 1.0);
    EXPECT_DOUBLE_EQ(v.y, -1.0);
}
