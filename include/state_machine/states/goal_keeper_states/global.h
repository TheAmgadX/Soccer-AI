#pragma once
#include "../../../game/entities/goal_keeper.h"
#include "../../state.h"

class GoalKeeperGlobal : public State<GoalKeeper> {
public:
    virtual void Enter(GoalKeeper* entity) override;
    virtual void Process(GoalKeeper* entity) override;
    virtual void Exit(GoalKeeper* entity) override;

    ~GoalKeeperGlobal() = default;

    virtual bool onMessage(GoalKeeper* Entity, const Message& msg) override;

    static GoalKeeperGlobal* Instance() {
        static GoalKeeperGlobal instance;
        return &instance;
    }
};
