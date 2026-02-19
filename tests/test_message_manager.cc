#include <gtest/gtest.h>
#include "state_machine/message_manager.h"
#include "game/entity_manager.h"
#include "game/base_game_entity.h"
#include "utils/time_system.h"
#include <vector>

// Mock Entity to capture messages
class MockReceiver : public Entity {
public:
    std::vector<Message> receivedMessages;

    MockReceiver() : Entity() {}

    void Update() override {}

    void HandleMessage(const Message& message) override {
        receivedMessages.push_back(message);
    }
};

class MessageManagerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Reset time
        Timer->g_Time = 0.0;
        Timer->g_TimeElapsed = 0.0;
    }

    void TearDown() override {
    }
};

TEST_F(MessageManagerTest, SendImmediateMessage) {
    MockReceiver* receiver = new MockReceiver();
    EntityManager::Instance()->AddEntity(receiver);

    int senderId = 999; // Dummy sender
    int msgType = 1;
    
    // Send with no delay
    MessageManager::Instance()->SendMessage(senderId, receiver->Id(), msgType, 0.0, nullptr);

    ASSERT_EQ(receiver->receivedMessages.size(), 1);
    EXPECT_EQ(receiver->receivedMessages[0].msg, msgType);
    EXPECT_EQ(receiver->receivedMessages[0].sender, senderId);
    EXPECT_EQ(receiver->receivedMessages[0].receiver, receiver->Id());

    // Clean up
    delete receiver; 
}

TEST_F(MessageManagerTest, SendDelayedMessage) {
    MockReceiver* receiver = new MockReceiver();
    EntityManager::Instance()->AddEntity(receiver);

    int senderId = 999;
    int msgType = 2;
    double delay = 2.0;

    // Send with delay
    MessageManager::Instance()->SendMessage(senderId, receiver->Id(), msgType, delay, nullptr);

    // Should NOT be received yet
    EXPECT_EQ(receiver->receivedMessages.size(), 0);

    // Advance time by 1.0 (total 1.0) -> still waiting
    Timer->g_Time = 1.0;
    MessageManager::Instance()->SendDelayedMessages();
    EXPECT_EQ(receiver->receivedMessages.size(), 0);

    // Advance time by 1.1 (total 2.1) -> should trigger
    Timer->g_Time = 2.1;
    MessageManager::Instance()->SendDelayedMessages();
    
    ASSERT_EQ(receiver->receivedMessages.size(), 1);
    EXPECT_EQ(receiver->receivedMessages[0].msg, msgType);
    
    delete receiver;
}

TEST_F(MessageManagerTest, SendMessageToNonExistentEntity) {
    // Should fail gracefully (no crash)
    MessageManager::Instance()->SendMessage(1, -12345, 1, 0.0, nullptr);
    // If no crash, pass.
}
