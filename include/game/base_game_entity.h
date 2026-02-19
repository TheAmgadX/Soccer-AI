#pragma once 
#include "math/vector.h"
#include "state_machine/message.h"

class Entity {
    int m_Id;
    bool m_Tag;

    int nextId(){
        static int id = 1;

        return id++;
    }

public:
    Vector m_Pos;
    double m_Radius;


    Entity() : m_Id(nextId()), m_Tag(false), m_Pos(Vector()), m_Radius(0.0) {}

    Entity(Vector _pos, double _radius) : m_Id(nextId()), m_Tag(false), m_Pos(_pos), m_Radius(_radius) {}

    virtual ~Entity() {}

    virtual void Update() = 0;

    virtual void HandleMessage(const Message& message) = 0;

    Vector Pos() const {
        return m_Pos;
    }

    double Radius() const {
        return m_Radius;
    }

    int Id() const {
        return m_Id;
    }
    
    void Tag() {
        m_Tag = true;
    }

    void UnTag() {
        m_Tag = false;
    }

    bool GetTag() const {
        return m_Tag;
    }
};