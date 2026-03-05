#pragma once
#include "../../../game/entities/field_player.h"
#include "../../state.h"

class FieldPlayerChase : public State<FieldPlayer> {
private:
    FieldPlayerChase() {}

public:
    ~FieldPlayerChase();

    virtual void Enter(FieldPlayer* Entity) override;
    virtual void Exit(FieldPlayer* Entity) override;
    virtual void Process(FieldPlayer* Entity) override;

    static inline FieldPlayerChase* Instance() {
        static FieldPlayerChase instance;
        return &instance;
    }
};
