#pragma once
#include "../../../game/entities/goal_keeper.h"
#include "../../state.h"

class GoalKeeperPutBallBackInPlay : public State<GoalKeeper> {
public:
    virtual void Enter(GoalKeeper* entity) override;
    virtual void Process(GoalKeeper* entity) override;
    virtual void Exit(GoalKeeper* entity) override;

    ~GoalKeeperPutBallBackInPlay() = default;

    static GoalKeeperPutBallBackInPlay* Instance() {
        static GoalKeeperPutBallBackInPlay instance;
        return &instance;
    }
};
