#pragma once
#include "../../../game/entities/field_player.h"
#include "../../state.h"

class FieldPlayerReceive : public State<FieldPlayer> {
private:
    FieldPlayerReceive() {}

public:
    ~FieldPlayerReceive();

    virtual void Enter(FieldPlayer* Entity) override;
    virtual void Exit(FieldPlayer* Entity) override;
    virtual void Process(FieldPlayer* Entity) override;

    static inline FieldPlayerReceive* Instance() {
        static FieldPlayerReceive instance;
        return &instance;
    }
};
