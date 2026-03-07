#include "game/soccer_team.h"
#include "game/entities/field_player.h"
#include "state_machine/message.h"
#include "state_machine/message_manager.h"
#include "state_machine/states/field_player_states/support.h"
#include "state_machine/states/soccer_team_states/attacking.h"
#include "state_machine/states/soccer_team_states/defending.h"
#include "state_machine/states/soccer_team_states/prepare_kick_off.h"
#include "state_machine/states/field_player_states/chase.h"
#include "utils/constants.h"
#include <cstddef>

namespace {
void ChangeHomeRegion(
    SoccerTeam *team, const int new_regions[constants::PLAYERS_PER_TEAM]) {
    for (size_t i = 0; i < constants::PLAYERS_PER_TEAM; ++i) {
        team->m_Players[i]->SetHomeRegion(new_regions[i]);
    }
}

FieldPlayer *ClosestFieldPlayerToBall(SoccerTeam *team) {
    FieldPlayer *closest = nullptr;
    double closest_distance_sq = MAXFLOAT;

    for (size_t i = 1; i < team->m_Players.size(); ++i) {
        FieldPlayer *field_player = static_cast<FieldPlayer *>(team->m_Players[i]);
        const double distance_sq =
            VecDistanceSQ(field_player->Pos(), team->Pitch()->p_Ball->Pos());

        if (distance_sq < closest_distance_sq) {
            closest_distance_sq = distance_sq;
            closest = field_player;
        }
    }

    return closest;
}

void AssignSupportPlayersByBallProximity(SoccerTeam *team) {
    Player *primary = nullptr;
    Player *secondary = nullptr;
    double primary_dist_sq = MAXFLOAT;
    double secondary_dist_sq = MAXFLOAT;

    for (size_t i = 1; i < team->m_Players.size(); ++i) {
        Player *candidate = team->m_Players[i];
        if (candidate == team->ControllingPlayer()) {
            continue;
        }

        const double dist_sq =
            VecDistanceSQ(candidate->Pos(), team->Pitch()->p_Ball->Pos());

        if (dist_sq < primary_dist_sq) {
            secondary = primary;
            secondary_dist_sq = primary_dist_sq;
            primary = candidate;
            primary_dist_sq = dist_sq;
        } else if (dist_sq < secondary_dist_sq) {
            secondary = candidate;
            secondary_dist_sq = dist_sq;
        }
    }

    team->SetSupportingPlayer(primary);
    team->SetSecondarySupportingPlayer(secondary);
}
} // namespace

// ---------------------------------------
// -------------- Attacking --------------
// ---------------------------------------
void Attacking::Enter(SoccerTeam *team) {
    const int blue_regions[constants::PLAYERS_PER_TEAM] = {1, 12, 14, 6, 4};
    const int red_regions[constants::PLAYERS_PER_TEAM] = {16, 3, 5, 9, 13};

    switch (team->Color()) {
    case SoccerTeam::TeamColor::BLUE:
        ChangeHomeRegion(team, blue_regions);
        break;
    case SoccerTeam::TeamColor::RED:
        ChangeHomeRegion(team, red_regions);
        break;
    }

    AssignSupportPlayersByBallProximity(team);
    team->UpdateSupportSpots();

    if (team->SupportingPlayer()) {
        MsgMgr->SendMessage(constants::GAME_ID, team->SupportingPlayer()->Id(),
                            MessageEnum::msgSupport, constants::SEND_MESSAGE_NOW, nullptr);
    }

    if (team->SecondarySupportingPlayer()) {
        MsgMgr->SendMessage(constants::GAME_ID,
                            team->SecondarySupportingPlayer()->Id(),
                            MessageEnum::msgSupport, constants::SEND_MESSAGE_NOW, nullptr);
    }
}

void Attacking::Exit(SoccerTeam *team) {
    team->setControllingPlayer(nullptr);
    team->SetSupportingPlayer(nullptr);
    team->SetSecondarySupportingPlayer(nullptr);
}

void Attacking::Process(SoccerTeam *team) {
    if (!team->InControl()) {
        team->FSM()->ChangeState(Defending::Instance());
        return;
    }

    team->UpdateSupportSpots();

    if (team->SupportingPlayer()) {
        MsgMgr->SendMessage(constants::GAME_ID, team->SupportingPlayer()->Id(),
                            MessageEnum::msgSupport, constants::SEND_MESSAGE_NOW, nullptr);
    }

    if (team->SecondarySupportingPlayer()) {
        MsgMgr->SendMessage(constants::GAME_ID,
                            team->SecondarySupportingPlayer()->Id(),
                            MessageEnum::msgSupport, constants::SEND_MESSAGE_NOW, nullptr);
    }
}

// ---------------------------------------
// -------------- Defending --------------
// ---------------------------------------
void Defending::Enter(SoccerTeam *team) {
    const int blue_regions[constants::PLAYERS_PER_TEAM] = {1, 6, 8, 3, 5};
    const int red_regions[constants::PLAYERS_PER_TEAM] = {16, 9, 11, 12, 14};

    switch (team->Color()) {
    case SoccerTeam::TeamColor::BLUE:
        ChangeHomeRegion(team, blue_regions);
        break;
    case SoccerTeam::TeamColor::RED:
        ChangeHomeRegion(team, red_regions);
        break;
    }
}

void Defending::Exit(SoccerTeam *team) {}

void Defending::Process(SoccerTeam *team) {
    if (team->InControl()) {
        team->FSM()->ChangeState(Attacking::Instance());
        return;
    }
}

// --------------------------------------------------
// -------------- Prepare For Kick Off --------------
// --------------------------------------------------
void PrepareForKickOff::Enter(SoccerTeam *team) {
    team->setControllingPlayer(nullptr);
    team->SetReceivingPlayer(nullptr);
    team->SetSupportingPlayer(nullptr);
    team->SetClosestPlayerToBall(nullptr);

    team->ReturnFieldPlayersToHome();
}

void PrepareForKickOff::Exit(SoccerTeam *team) { (void)team; }

void PrepareForKickOff::Process(SoccerTeam *team) {
    if (!team->AllPlayersAtHome() || !team->Opponent()->AllPlayersAtHome()) {
        return;
    }

    if (!team->Opponent()->FSM()->IsInState(PrepareForKickOff::Instance())) {
        return;
    }

    FieldPlayer *home_nearest = ClosestFieldPlayerToBall(team);
    FieldPlayer *away_nearest = ClosestFieldPlayerToBall(team->Opponent());

    FieldPlayer *kickoff_player = home_nearest;
    SoccerTeam *kickoff_team = team;

    if (home_nearest && away_nearest) {
        const double home_dist_sq =
            VecDistanceSQ(home_nearest->Pos(), team->Pitch()->p_Ball->Pos());
        const double away_dist_sq =
            VecDistanceSQ(away_nearest->Pos(), team->Pitch()->p_Ball->Pos());

        if (away_dist_sq < home_dist_sq) {
            kickoff_player = away_nearest;
            kickoff_team = team->Opponent();
        }
    } else if (!home_nearest && away_nearest) {
        kickoff_player = away_nearest;
        kickoff_team = team->Opponent();
    }

    team->setControllingPlayer(nullptr);
    team->Opponent()->setControllingPlayer(nullptr);

    if (kickoff_player) {
        kickoff_team->setControllingPlayer(kickoff_player);
        kickoff_player->FSM()->ChangeState(FieldPlayerChase::Instance());
    }

    team->FSM()->ChangeState(Attacking::Instance());
    team->Opponent()->FSM()->ChangeState(Attacking::Instance());
    team->Pitch()->SetGameOn(true);
}
