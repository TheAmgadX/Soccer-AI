#pragma once

#include "../../../game/soccer_team.h"
#include "../../state.h"

class Attacking : public State<SoccerTeam> {
    Attacking() = default;

public:
    virtual void Enter(SoccerTeam *pTeam) override;
    virtual void Process(SoccerTeam *pTeam) override;
    virtual void Exit(SoccerTeam *pTeam) override;

    ~Attacking() = default;

    static Attacking *Instance() {
        static Attacking instance;
        return &instance;
    }
};
