#pragma once

#include "player.h"
#include "../../state_machine/state_machine.h"

class FieldPlayer : public Player {
    StateMachine<FieldPlayer>* p_StateMachine;

    double kick_limit;

public:
    FieldPlayer();

    ~FieldPlayer();

    virtual void Update() override;

    StateMachine<FieldPlayer>* FSM() const { return p_StateMachine; }

};
