#pragma once

#include "../../math/vector.h"
#include "../moving_entity.h"

class Player : public MovingEntity {
public:
    Player() : MovingEntity() {}

    Player(Vector _pos, double _radius, Vector _velocity, Vector _heading,
        double max_speed, double max_force, double turn_rate, TimeSystem *_timer) :
        MovingEntity(_pos, _radius, _velocity, _heading, max_speed, max_force, turn_rate, _timer) {}

    virtual ~Player() = 0;

    inline void Update() override {}

    inline void HandleMessage(const Message &message) override {}

    inline Vector FuturePos(const double time) const {
        return m_Pos + m_Velocity * time;
    }

    virtual bool InHome() const = 0;
};
