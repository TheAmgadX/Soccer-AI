#include "game/soccer_team.h"
#include "state_machine/states/soccer_team_states/attacking.h"
#include "state_machine/states/soccer_team_states/defending.h"
#include "state_machine/states/soccer_team_states/prepare_kick_off.h"
#include "utils/constants.h"
#include <cstddef>


void ChangeHomeRegion(SoccerTeam* team, const int new_regions[constants::PLAYERS_PER_TEAM]) {
    for (size_t i = 0; i < constants::PLAYERS_PER_TEAM; ++i) {
        team->m_Players[i]->SetHomeRegion(new_regions[i]);
    }
}

// ---------------------------------------
// -------------- Attacking --------------
// ---------------------------------------
void Attacking::Enter(SoccerTeam* team) {
    const int blue_regions[constants::PLAYERS_PER_TEAM] = { 1, 12, 14, 6, 4 };
    const int red_regions[constants::PLAYERS_PER_TEAM] = { 16, 3, 5, 9, 13 };

    switch (team->Color()) {
        case SoccerTeam::TeamColor::BLUE:
            ChangeHomeRegion(team, blue_regions);
            break;
        case SoccerTeam::TeamColor::RED:
            ChangeHomeRegion(team, red_regions);
            break;
    }
}

void Attacking::Exit(SoccerTeam* team) {
    team->SetSupportingPlayer(nullptr);
}

void Attacking::Process(SoccerTeam* team) {
    if(!team->InControl()) {
        team->FSM().ChangeState(Defending::Instance());
        return;
    }

    team->DetermineBestSupportingPlayer();
}

// ---------------------------------------
// -------------- Defending --------------
// ---------------------------------------
void Defending::Enter(SoccerTeam* team) {
    const int blue_regions[constants::PLAYERS_PER_TEAM] = { 1, 6, 8, 3, 5 };
    const int red_regions[constants::PLAYERS_PER_TEAM] = { 16, 9, 11, 12, 14 };

    switch (team->Color()) {
        case SoccerTeam::TeamColor::BLUE:
            ChangeHomeRegion(team, blue_regions);
            break;
        case SoccerTeam::TeamColor::RED:
            ChangeHomeRegion(team, red_regions);
            break;
    }
}

void Defending::Exit(SoccerTeam* team) {}

void Defending::Process(SoccerTeam* team) {
    if (team->InControl()) {
        team->FSM().ChangeState(Attacking::Instance());
        return;
    }
}



// --------------------------------------------------
// -------------- Prepare For Kick Off --------------
// --------------------------------------------------
void PrepareForKickOff::Enter(SoccerTeam* team) {
    team->setControllingPlayer(nullptr);
    team->SetReceivingPlayer(nullptr);
    team->SetSupportingPlayer(nullptr);
    team->SetClosestPlayerToBall(nullptr);

    team->ReturnFieldPlayersToHome();
}

void PrepareForKickOff::Exit(SoccerTeam* team) {
    team->Pitch()->SetGameOn(true);
}

void PrepareForKickOff::Process(SoccerTeam* team) {
    if(team->AllPlayersAtHome() && team->Opponent()->AllPlayersAtHome()) {
        team->FSM().ChangeState(Defending::Instance());
    }
}
