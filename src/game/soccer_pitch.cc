#include "game/soccer_pitch.h"
#include "utils/constants.h"

SoccerPitch::SoccerPitch() {}

void SoccerPitch::Update() {
    if (m_GamePause)
        return;

    p_Ball->Update();

    // p_BlueTeam->Update();
    // p_RedTeam->Update();

    // if (p_RedGoal->Scored(p_Ball) || p_BlueGoal->Scored(p_Ball)) {
    //     m_GameOn = false;

    //     p_Ball->PlaceAtPos(Vector((double)constants::PITCH_WIDTH / 2.0,
    //                             (double)constants::PITCH_HEIGHT / 2.0));

    //     // TODO: make the states and finish the FSM.
    //     p_RedTeam->FSM()->ChangeState(PrepareForKickOff::Instance());
    //     p_BlueTeam->FSM()->ChangeState(PrepareForKickOff::Instance());
    // }
}

void SoccerPitch::CreatePitchRegions() {}

SoccerPitch::~SoccerPitch() {}
