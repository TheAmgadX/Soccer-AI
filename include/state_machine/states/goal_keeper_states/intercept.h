#pragma once
#include "../../../game/entities/goal_keeper.h"
#include "../../state.h"

class GoalKeeperIntercept : public State<GoalKeeper> {
public:
    virtual void Enter(GoalKeeper* entity) override;
    virtual void Process(GoalKeeper* entity) override;
    virtual void Exit(GoalKeeper* entity) override;

    ~GoalKeeperIntercept() = default;

    static GoalKeeperIntercept* Instance() {
        static GoalKeeperIntercept instance;
        return &instance;
    }
};
