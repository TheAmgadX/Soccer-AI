#pragma once
#include "../../../game/entities/field_player.h"
#include "../../state.h"

class FieldPlayerGlobal : public State<FieldPlayer> {
private:
    FieldPlayerGlobal() {}

public:
    ~FieldPlayerGlobal() = default;

    virtual void Enter(FieldPlayer* Entity) override;
    virtual void Exit(FieldPlayer* Entity) override;
    virtual void Process(FieldPlayer* Entity) override;

    virtual bool onMessage(FieldPlayer* Entity, const Message& msg) override;


    static inline FieldPlayerGlobal* Instance() {
        static FieldPlayerGlobal instance;
        return &instance;
    }
};
