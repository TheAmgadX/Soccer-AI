#include <gtest/gtest.h>
#include "state_machine/message.h"

TEST(MessageTest, DefaultConstructor) {
    Message m;
    EXPECT_EQ(m.sender, -1);
    EXPECT_EQ(m.receiver, -1);
    EXPECT_EQ(m.msg, -1);
    EXPECT_DOUBLE_EQ(m.time, 0.0);
    EXPECT_EQ(m.extraInfo, nullptr);
}

TEST(MessageTest, ParameterizedConstructor) {
    int sender = 1;
    int receiver = 2;
    int msgType = 100;
    double dispatchTime = 123.45;
    void* info = (void*)0xDEADBEEF;

    Message m(sender, receiver, msgType, dispatchTime, info);

    EXPECT_EQ(m.sender, sender);
    EXPECT_EQ(m.receiver, receiver);
    EXPECT_EQ(m.msg, msgType);
    EXPECT_DOUBLE_EQ(m.time, dispatchTime);
    EXPECT_EQ(m.extraInfo, info);
}

TEST(MessageTest, EqualityOperator) {
    // Current operator== only checks time
    Message m1(1, 2, 3, 10.0, nullptr);
    Message m2(4, 5, 6, 10.0, nullptr);
    EXPECT_TRUE(m1 == m2);

    Message m3(1, 2, 3, 10.1, nullptr);
    EXPECT_FALSE(m1 == m3);
}

TEST(MessageTest, ComparisonOperators) {
    Message mEarlier(0, 0, 0, 10.0, nullptr);
    Message mLater(0, 0, 0, 20.0, nullptr);

    EXPECT_TRUE(mEarlier < mLater);
    EXPECT_FALSE(mLater < mEarlier);
    
    EXPECT_TRUE(mLater > mEarlier);
    EXPECT_FALSE(mEarlier > mLater);

    // Strict weak ordering check
    Message mSameTime(0,0,0, 10.0, nullptr);
    EXPECT_FALSE(mSameTime < mEarlier);
    EXPECT_FALSE(mEarlier < mSameTime);
}
