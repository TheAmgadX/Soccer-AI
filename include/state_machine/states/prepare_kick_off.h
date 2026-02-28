#pragma once

#include "../../game/soccer_team.h"
#include "../state.h"

// TODO: implement the methods
class PrepareForKickOff : public State<SoccerTeam> {
    PrepareForKickOff() = default;

public:
    virtual void Enter(SoccerTeam *pTeam) override {}
    virtual void Process(SoccerTeam *pTeam) override {}
    virtual void Exit(SoccerTeam *pTeam) override {}

    ~PrepareForKickOff() = default;

    static PrepareForKickOff *Instance() {
        static PrepareForKickOff instance;
        return &instance;
    }
};
