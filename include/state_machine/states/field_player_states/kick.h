#pragma once
#include "../../../game/entities/field_player.h"
#include "../../state.h"

class FieldPlayerKick : public State<FieldPlayer> {
private:
    FieldPlayerKick() {}

public:
    ~FieldPlayerKick();

    virtual void Enter(FieldPlayer* Entity) override;
    virtual void Exit(FieldPlayer* Entity) override;
    virtual void Process(FieldPlayer* Entity) override;

    static inline FieldPlayerKick* Instance() {
        static FieldPlayerKick instance;
        return &instance;
    }
};
