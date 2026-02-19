#include <gtest/gtest.h>
#include "game/entity_manager.h"
#include "game/base_game_entity.h"

// Minimal concrete implementation for testing
class TestEntityMgrImpl : public Entity {
public:
    TestEntityMgrImpl() : Entity() {}
    void Update() override {}
    void HandleMessage(const Message& message) override {}
};

TEST(EntityManagerTest, Instance) {
    EntityManager* mgr1 = EntityManager::Instance();
    EntityManager* mgr2 = EntityManager::Instance();
    EXPECT_EQ(mgr1, mgr2);
    EXPECT_NE(mgr1, nullptr);
}

TEST(EntityManagerTest, AddAndGetEntity) {
    EntityManager* mgr = EntityManager::Instance();
    
    TestEntityMgrImpl* e1 = new TestEntityMgrImpl();
    TestEntityMgrImpl* e2 = new TestEntityMgrImpl();
    
    mgr->AddEntity(e1);
    mgr->AddEntity(e2);
    
    EXPECT_EQ(mgr->GetEntity(e1->Id()), e1);
    EXPECT_EQ(mgr->GetEntity(e2->Id()), e2);
    
    // Clean up
    delete e1;
    delete e2;
}

TEST(EntityManagerTest, GetNonExistentEntity) {
    EntityManager* mgr = EntityManager::Instance();
    
    int invalidId = -999;
    Entity* result = mgr->GetEntity(invalidId);
    EXPECT_EQ(result, nullptr); 
}
