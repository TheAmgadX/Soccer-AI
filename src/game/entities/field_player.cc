#include "game/entities/field_player.h"
#include "game/steering_behavs.h"
#include "math/vector.h"
#include "state_machine/state_machine.h"
#include "state_machine/states/field_player_states/global.h"
#include "state_machine/states/field_player_states/return_to_home.h"
#include "utils/constants.h"

FieldPlayer::FieldPlayer(SoccerTeam* team, int home_region, Vector _heading) : Player(team, home_region, _heading) {
    p_StateMachine = new StateMachine<FieldPlayer>(this, FieldPlayerReturnToHome::Instance(), FieldPlayerGlobal::Instance());

    p_StateMachine->CurrentState()->Enter(this);
}

FieldPlayer::~FieldPlayer() {
    delete p_StateMachine;
}

void FieldPlayer::Update() {
    p_StateMachine->Update();

    Vector steering_force = Steering()->Calculate();

    m_Velocity += steering_force * Timer->g_TimeElapsed;

    m_Velocity.Truncate(constants::PLAYER_MAX_SPEED);

    if(!m_Velocity.isZero()){
        m_Heading = VecNormalize(m_Velocity);
        m_Side = m_Heading.Perp();
    }

    m_Pos += m_Velocity * Timer->g_TimeElapsed;
    ValidatePitchBoundaries(m_Pos);
}

void FieldPlayer::HandleMessage(const Message &message) {
    if (p_StateMachine->GlobalState() &&
        p_StateMachine->GlobalState()->onMessage(this, message)) {
        return;
    }

    if (p_StateMachine->CurrentState()) {
        p_StateMachine->CurrentState()->onMessage(this, message);
    }
}
