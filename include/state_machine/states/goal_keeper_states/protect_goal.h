#pragma once
#include "../../../game/entities/goal_keeper.h"
#include "../../state.h"

class GoalKeeperProtectGoal : public State<GoalKeeper> {
public:
    virtual void Enter(GoalKeeper* entity) override;
    virtual void Process(GoalKeeper* entity) override;
    virtual void Exit(GoalKeeper* entity) override;

    ~GoalKeeperProtectGoal() = default;

    static GoalKeeperProtectGoal* Instance() {
        static GoalKeeperProtectGoal instance;
        return &instance;
    }
};
