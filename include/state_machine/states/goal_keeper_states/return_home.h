#pragma once
#include "../../../game/entities/goal_keeper.h"
#include "../../state.h"

class GoalKeeperReturnHome : public State<GoalKeeper> {
public:
    virtual void Enter(GoalKeeper* entity) override;
    virtual void Process(GoalKeeper* entity) override;
    virtual void Exit(GoalKeeper* entity) override;

    ~GoalKeeperReturnHome() = default;

    static GoalKeeperReturnHome* Instance() {
        static GoalKeeperReturnHome instance;
        return &instance;
    }
};
