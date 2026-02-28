#pragma once
#include "state.h"
#include <assert.h>

template <class entity_type>
class StateMachine {
    entity_type *p_Owner;

    State<entity_type> *p_CurrentState;

    State<entity_type> *p_GlobalState;

public:
    StateMachine()
        : p_Owner(nullptr), p_CurrentState(nullptr), p_GlobalState(nullptr) {}

    StateMachine(entity_type *_owner, State<entity_type> *_state,
                State<entity_type> *_global)
        : p_Owner(_owner), p_CurrentState(_state), p_GlobalState(_global) {}

    ~StateMachine() {
        delete p_CurrentState;
        delete p_GlobalState;
    }

    entity_type *CurrentState() { return p_CurrentState; }

    entity_type *GlobalState() { return p_GlobalState; }

    void Update() const {
        if (p_GlobalState) {
        p_GlobalState->Process(p_Owner);
        }

        if (p_CurrentState) {
        p_CurrentState->Process(p_Owner);
        }
    }

    void ChangeState(State<entity_type> *_state) {
        assert(_state && "state ptr cannot be nullptr");

        p_CurrentState->Exit(p_Owner);

        p_CurrentState = _state;

        p_CurrentState->Enter(p_Owner);
    }
};
