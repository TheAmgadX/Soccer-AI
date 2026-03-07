#pragma once
#include "../../../game/entities/field_player.h"
#include "../../state.h"

class FieldPlayerSupport : public State<FieldPlayer> {
private:
    FieldPlayerSupport() {}

public:
    ~FieldPlayerSupport() = default;

    virtual void Enter(FieldPlayer* Entity) override;
    virtual void Exit(FieldPlayer* Entity) override;
    virtual void Process(FieldPlayer* Entity) override;

    static inline FieldPlayerSupport* Instance() {
        static FieldPlayerSupport instance;
        return &instance;
    }
};
