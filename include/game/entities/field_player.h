#pragma once

#include "player.h"
#include "../../state_machine/state_machine.h"

class FieldPlayer : public Player {
    StateMachine<FieldPlayer>* p_StateMachine;

    double kick_limit;

public:
    FieldPlayer(SoccerTeam* team, int home_region, Vector _heading);

    ~FieldPlayer();

    virtual void Update() override;
    virtual void HandleMessage(const Message &message) override;

    StateMachine<FieldPlayer>* FSM() const { return p_StateMachine; }

};
