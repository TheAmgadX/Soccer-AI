#pragma once


template <class entity_type>
class State {
public:
    virtual ~State() {}

    virtual void Enter(entity_type* Entity) = 0;
    virtual void Process(entity_type* Entity) = 0;
    virtual void Exit(entity_type* Entity) = 0;
};