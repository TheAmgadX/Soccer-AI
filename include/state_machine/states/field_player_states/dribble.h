#pragma once
#include "../../../game/entities/field_player.h"
#include "../../state.h"

class FieldPlayerDribble : public State<FieldPlayer> {
private:
    FieldPlayerDribble() {}

public:
    ~FieldPlayerDribble() = default;

    virtual void Enter(FieldPlayer* Entity) override;
    virtual void Exit(FieldPlayer* Entity) override;
    virtual void Process(FieldPlayer* Entity) override;

    static inline FieldPlayerDribble* Instance() {
        static FieldPlayerDribble instance;
        return &instance;
    }
};
