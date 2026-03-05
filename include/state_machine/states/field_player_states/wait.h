#pragma once
#include "../../../game/entities/field_player.h"
#include "../../state.h"

class FieldPlayerWait : public State<FieldPlayer> {
private:
    FieldPlayerWait() {}

public:
    ~FieldPlayerWait();

    virtual void Enter(FieldPlayer* Entity) override;
    virtual void Exit(FieldPlayer* Entity) override;
    virtual void Process(FieldPlayer* Entity) override;

    static inline FieldPlayerWait* Instance() {
        static FieldPlayerWait instance;
        return &instance;
    }
};
