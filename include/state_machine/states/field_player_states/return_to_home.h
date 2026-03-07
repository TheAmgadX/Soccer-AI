#pragma once
#include "../../../game/entities/field_player.h"
#include "../../state.h"

class FieldPlayerReturnToHome : public State<FieldPlayer> {
private:
    FieldPlayerReturnToHome() {}

public:
    ~FieldPlayerReturnToHome() = default;

    virtual void Enter(FieldPlayer* Entity) override;
    virtual void Exit(FieldPlayer* Entity) override;
    virtual void Process(FieldPlayer* Entity) override;

    static inline FieldPlayerReturnToHome* Instance() {
        static FieldPlayerReturnToHome instance;
        return &instance;
    }
};
