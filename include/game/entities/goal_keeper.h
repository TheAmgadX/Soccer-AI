#pragma once

#include "../../math/vector.h"
#include "player.h"
#include "../../state_machine/state_machine.h"

class GoalKeeper : public Player {
    StateMachine<GoalKeeper>* p_StateMachine;
public:
    GoalKeeper(SoccerTeam* team, int home_region, Vector _heading);
    ~GoalKeeper();

    virtual void Update() override;

    inline StateMachine<GoalKeeper>* FSM() const { return p_StateMachine; }

    Vector GetRearInterposeTarget() const;

    bool BallWithinKeeperRange() const;

    bool BallWithinInterceptRange() const;
};
