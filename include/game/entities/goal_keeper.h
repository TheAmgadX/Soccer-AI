#pragma once

#include "game/entities/player.h"

class GoalKeeper : public Player {
public:
    GoalKeeper();
    ~GoalKeeper();

    virtual void Update() override;
};
