#pragma once
#include "base_game_entity.h"
#include <unordered_map>

using EntityMap = std::unordered_map<int, Entity*>;

class EntityManager {
    EntityMap m_EntityMap;
    
    EntityManager() {}

    // copy ctor and assignment should be private
    EntityManager(const EntityManager &);
    EntityManager &operator =(const EntityManager &);
public:
    static EntityManager* Instance(){
        static EntityManager mgr;
        
        return &mgr;
    }
    
    void AddEntity(Entity* entity) {
        if(!entity) return;
        
        m_EntityMap[entity->Id()] = entity;
    }
    
    Entity* GetEntity(int id) {
        return m_EntityMap[id];
    }
};  

#define EntityMgr EntityManager::Instance();
