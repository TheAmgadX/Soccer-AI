#pragma once

#include "player.h"

class FieldPlayer : public Player {
public:
    FieldPlayer();
    ~FieldPlayer();

    virtual bool InHome() const override;

    virtual void Update() override;
};
