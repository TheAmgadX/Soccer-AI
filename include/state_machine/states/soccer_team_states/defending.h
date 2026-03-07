#pragma once

#include "../../../game/soccer_team.h"
#include "../../state.h"

class Defending : public State<SoccerTeam> {
    Defending() = default;

public:
    virtual void Enter(SoccerTeam *pTeam) override;
    virtual void Process(SoccerTeam *pTeam) override;
    virtual void Exit(SoccerTeam *pTeam) override;

    ~Defending() = default;

    static Defending *Instance() {
        static Defending instance;
        return &instance;
    }
};
