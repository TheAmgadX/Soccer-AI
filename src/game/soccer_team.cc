    #include "game/soccer_team.h"
    #include "game/entities/field_player.h"
    #include "game/entities/goal_keeper.h"
    #include "game/entities/player.h"
    #include "math/vector.h"
    #include "state_machine/message.h"
    #include "state_machine/message_manager.h"
    #include "state_machine/states/soccer_team_states/prepare_kick_off.h"
    #include "utils/constants.h"
    #include "utils/time_system.h"
    #include "utils/utils.h"
    #include <cmath>
    #include <cstddef>
    #include <cstdint>
    #include <cstdlib>

SoccerTeam::~SoccerTeam() {
    for (std::size_t i = 0; i < m_Players.size(); ++i) {
        delete m_Players[i];
    }

    delete p_StateMachine;
}

void SoccerTeam::CreatePlayers() {
    m_Players.reserve(constants::PLAYERS_PER_TEAM);

    int goal_keeper_home_region;
    std::vector<int> players_home_region;
    players_home_region.reserve(constants::PLAYERS_PER_TEAM - 1);

    switch (Color()) {
        case RED:
            goal_keeper_home_region = 16;
            players_home_region = {12, 14, 9, 11};
            break;
        case BLUE:
            goal_keeper_home_region = 1;
            players_home_region = {3, 5, 6, 8};
            break;
    }

    p_GoalKeeper = new GoalKeeper(this, goal_keeper_home_region, Vector());
    m_Players.push_back(p_GoalKeeper);

    for (int i = 0; i < constants::PLAYERS_PER_TEAM - 1; ++i) {
        m_Players.emplace_back(new FieldPlayer(this, players_home_region[i], Vector()));
    }
}

void SoccerTeam::Initialize(SoccerTeam *opponent) {
    p_StateMachine = new StateMachine<SoccerTeam>(
        this, PrepareForKickOff::Instance(), nullptr);
    p_StateMachine->CurrentState()->Enter(this);

    CreatePlayers();
    p_Opponent = opponent;
}

void SoccerTeam::Update() {
    CalcClosestPlayerToBall();

    // Drop stale control if the "controlling" player no longer has playing contact
    // with the ball (e.g., after a pass or interception).
    if (p_ControllingPlayer && !p_ControllingPlayer->BallWithinKickingRange() &&
        !p_Pitch->GoalKeeperHasBall()) {
        p_ControllingPlayer = nullptr;
    }

    FSM()->Update();

    if (p_ControllingPlayer) {
        DetermineBestSupportingPlayer();
    }

    // Keep team shape: all non-active field players should return to home region.
    for (size_t i = 1; i < m_Players.size(); ++i) {
        Player *player = m_Players[i];

        if (player == p_ControllingPlayer || player == p_PlayerClosestToBall ||player == p_ReceivingPlayer
            || player == p_SupportingPlayer || player == p_SecondarySupportingPlayer) {
            continue;
        }

        MsgMgr->SendMessage(constants::GAME_ID, player->Id(),
                            MessageEnum::msgGotoHome, constants::SEND_MESSAGE_NOW, nullptr);
    }

    for (auto &player : m_Players) {
        player->Update();
    }
}

void SoccerTeam::CalcClosestPlayerToBall() {
    double distanceSQ = MAXFLOAT;
    for (size_t i = 0; i < m_Players.size(); ++i) {
        Vector to_ball = p_Pitch->p_Ball->Pos() - m_Players[i]->Pos();
        double dist = to_ball.LengthSQ();

        if (dist < distanceSQ) {
            distanceSQ = dist;
            p_PlayerClosestToBall = m_Players[i];
        }
    }

    m_DistanceSQToBallFromClosestPlayer = std::sqrt(distanceSQ);
}

void SoccerTeam::DetermineBestSupportingPlayer() {
    Player *best_supporting_player = nullptr;
    Player *second_supporting_player = nullptr;
    double min_distance = MAXFLOAT;
    double second_min_distance = MAXFLOAT;

    for (uint8_t i = 1; i < constants::PLAYERS_PER_TEAM; ++i) {
        if (m_Players[i] == p_ControllingPlayer)
            continue;

        Vector to_spot = GetBestSupportSpot() - m_Players[i]->Pos();
        double distance = to_spot.LengthSQ();

        if (distance < min_distance) {
            second_min_distance = min_distance;
            second_supporting_player = best_supporting_player;
            min_distance = distance;
            best_supporting_player = m_Players[i];
        } else if (distance < second_min_distance) {
            second_min_distance = distance;
            second_supporting_player = m_Players[i];
        }
    }

    Player *old_primary = p_SupportingPlayer;
    Player *old_secondary = p_SecondarySupportingPlayer;

    p_SupportingPlayer = best_supporting_player;
    p_SecondarySupportingPlayer = second_supporting_player;

    auto still_supporting = [&](Player *p) {
        return p != nullptr && (p == p_SupportingPlayer || p == p_SecondarySupportingPlayer);
    };

    if (old_primary && !still_supporting(old_primary)) {
        MsgMgr->SendMessage(constants::GAME_ID, old_primary->Id(), MessageEnum::msgGotoHome, constants::SEND_MESSAGE_NOW, nullptr);
    }
    if (old_secondary && !still_supporting(old_secondary)) {
        MsgMgr->SendMessage(constants::GAME_ID, old_secondary->Id(), MessageEnum::msgGotoHome, constants::SEND_MESSAGE_NOW, nullptr);
    }

    if (p_SupportingPlayer && p_SupportingPlayer != old_primary && p_SupportingPlayer != old_secondary) {
        MsgMgr->SendMessage(constants::GAME_ID, p_SupportingPlayer->Id(), MessageEnum::msgSupport, constants::SEND_MESSAGE_NOW, nullptr);
    }

    if (p_SecondarySupportingPlayer && p_SecondarySupportingPlayer != old_primary && p_SecondarySupportingPlayer != old_secondary) {
        MsgMgr->SendMessage(constants::GAME_ID, p_SecondarySupportingPlayer->Id(), MessageEnum::msgSupport, constants::SEND_MESSAGE_NOW, nullptr);
    }
}

void SoccerTeam::CheckSpot(Spot &spot) const {
    if (!p_ControllingPlayer) {
        return;
    }

    if (Color() == RED && spot.position.x > constants::FIELD_CENTER_X) {
        return;
    }

    if (Color() == BLUE && spot.position.x < constants::FIELD_CENTER_X) {
        return;
    }

    if (IsPassSafeFromAllOpponents(spot.position, nullptr, constants::Max_Passing_Force)) {
        spot.rate += constants::SAFE_PASS_RATE;
    }

    Vector tmp;

    if (CanShoot(spot.position, constants::MAX_SHOT_FORCE, tmp)) {
        spot.rate += constants::CAN_SHOT_RATE;
    }

    // Prefer support spots at a useful distance from the controlling player.
    Vector to_support_spot = spot.position - p_ControllingPlayer->Pos();
    double len = to_support_spot.Length();

    if (len > constants::MIN_DISTANCE_TO_SUPPORT_PLAYER && len < constants::MAX_DISTANCE_TO_SUPPORT_PLAYER) {
        spot.rate += constants::DISTANCE_BETWEEN_PLAYERS_RATE;
    }
}

Vector SoccerTeam::GetBestSupportSpot() {
    if (Timer->g_Time - m_TimeSinceLastUpdateToSupportSpot < constants::UPDATE_SUPPORT_SPOT_INTERVAL) {
        return m_BestSupportSpot;
    }

    m_TimeSinceLastUpdateToSupportSpot = Timer->g_Time;

    Spot best_spot;
    best_spot.rate = -1;
    best_spot.position = p_Pitch->p_Ball->Pos();

    for (int i = 0; i < p_Pitch->m_Regions.size(); ++i) {
        Spot spot;
        spot.position = p_Pitch->m_Regions[i].Center();
        spot.rate = 1.0;

        CheckSpot(spot);

        if (spot.rate > best_spot.rate) {
            best_spot = spot;
        }
    }

    m_BestSupportSpot = best_spot.position;
    return m_BestSupportSpot;
}

Vector SoccerTeam::SupportSpotFor(const Player *player) const {
    Vector spot = m_BestSupportSpot;
    if (player == p_SecondarySupportingPlayer) {
        spot.y += (Color() == RED ? -30.0 : 30.0);
        ValidatePitchBoundaries(spot);
    }
    return spot;
}

void SoccerTeam::ReturnFieldPlayersToHome() {
    for (size_t i = 1; i < m_Players.size(); ++i) {
        MsgMgr->SendMessage(constants::GAME_ID, m_Players[i]->Id(),
                            MessageEnum::msgGotoHome, constants::SEND_MESSAGE_NOW, nullptr);
    }
}

bool SoccerTeam::AllPlayersAtHome() const {
    for (size_t i = 1; i < constants::PLAYERS_PER_TEAM; ++i) {
        if (!m_Players[i]->InHome()) {
            return false;
        }
    }

    return true;
}

bool SoccerTeam::IsPassSafeFromOpponent(const Vector &target,
                                        const Player *const reciever,
                                        const Player *const opponent,
                                        const double power) const {
    Vector ball_pos = p_Pitch->p_Ball->Pos();
    Vector to_target = target - ball_pos;
    Vector to_target_norm = VecNormalize(to_target);
    Vector perp_to_target = to_target_norm.Perp();

    Vector local_opponent_pos = PointToLocalSpace(opponent->Pos(), to_target_norm,
                                                    perp_to_target, ball_pos);

    // opponent is behine the ball pass path.
    // this assumes the ball speed is equal to or higher than the opponent's
    // speed.
    if (local_opponent_pos.x < 0) {
        return true;
    }

    double distance_from_opponent_to_target = VecDistanceSQ(opponent->Pos(), target);

    if (VecDistanceSQ(ball_pos, target) < distance_from_opponent_to_target) {
        if (reciever) {
            if (distance_from_opponent_to_target >
                VecDistanceSQ(reciever->Pos(), target)) {
                return true;
            }
            return false;
        } else {
            return true;
        }
    }

    // check if the opponent cannot intersect the ball path.
    double time_to_cover_opponent_x = p_Pitch->p_Ball->TimeToCoverDistance(
        Vector(0, 0), Vector(local_opponent_pos.x, 0), power);
    double distance_opponent_can_cover_in_time =
        opponent->MaxSpeed() * time_to_cover_opponent_x +
        p_Pitch->p_Ball->Radius();

    if (distance_opponent_can_cover_in_time > std::fabs(local_opponent_pos.y)) {
        return false;
    }

    return true;
    }

    bool SoccerTeam::IsPassSafeFromAllOpponents(const Vector &target,
                                                const Player *const reciever,
                                                const double power) const {
    for (size_t i = 0; i < constants::PLAYERS_PER_TEAM; ++i) {
        if (!IsPassSafeFromOpponent(target, reciever, p_Opponent->m_Players[i], power)) {
            return false;
        }
    }

    return true;
}

bool SoccerTeam::CanShoot(const Vector &ball_pos, const double power,
                        Vector &shotTarget) const {
    uint8_t attemps = constants::ATTEMPS_TO_FIND_SHOT_POS;

    while (attemps--) {
        shotTarget = p_AwayGoal->Center();

        double min_y = constants::GOAL_TOP_Y + p_Pitch->p_Ball->Radius();
        double max_y = constants::GOAL_BOTTOM_Y - p_Pitch->p_Ball->Radius();

        shotTarget.y = Utils::RandomInt(min_y, max_y);

        double time = p_Pitch->p_Ball->TimeToCoverDistance(ball_pos, shotTarget, power);

        if (time >= 0) {
            if (IsPassSafeFromAllOpponents(shotTarget, nullptr, power)) {
                return true;
            }
        }
    }

    return false;
    }

    void SoccerTeam::RequestPass(Player *requester) const {
    // add condition to make the ai not perfect.
    // simulate human behavior.
    if (Utils::RandomFloatClamped() > 0.5) {
        return;
    }

    if (IsPassSafeFromAllOpponents(requester->Pos(), requester,
                                    constants::Max_Passing_Force)) {
        MsgMgr->SendMessage(requester->Id(), p_ControllingPlayer->Id(),
                            MessageEnum::msgPassToMe, constants::SEND_MESSAGE_NOW,
                            requester);
    }
    }

    bool SoccerTeam::FindPass(const Player *const passer, Player *&receiver,
                            Vector &passTarget, const double power,
                            const double minPassDistance) const {
    /*
    * best pass is the pass that cannot be intercepted by the opponent
    * and closest to the opponent goal.
    *
    * Not the smartest logic for football but works fine.
    */

    double closest_to_goal_so_far = MAXFLOAT;
    Vector target;

    for (size_t i = 1; i < constants::PLAYERS_PER_TEAM; ++i) {
        if (passer == m_Players[i])
        continue;

        if (VecDistanceSQ(passer->Pos(), m_Players[i]->Pos()) <
            (minPassDistance * minPassDistance)) {
        continue;
        }

        if (!GetBestPassToReceiver(passer, m_Players[i], target, power)) {
        continue;
        }

        double distance_to_goal = std::fabs(p_AwayGoal->Center().x - target.x);

        if (distance_to_goal < closest_to_goal_so_far) {
        closest_to_goal_so_far = distance_to_goal;
        passTarget = target;
        receiver = m_Players[i];
        }
    }

    // if no pass found.
    if (!receiver) {
        return false;
    }

        return true;
    }

bool SoccerTeam::GetBestPassToReceiver(const Player *const passer,
                                    const Player *const reciever,
                                    Vector &passTarget,
                                    const double power) const {

    double time = p_Pitch->p_Ball->TimeToCoverDistance(passer->Pos(),
                                                        reciever->Pos(), power);

    // ball cannot reach the reciever
    if (time < 0.0)
        return false;

    double receiver_range = time * reciever->MaxSpeed();

    double closest_to_goal_so_far = MAXFLOAT;

    bool found = false;

    if (IsPassSafeFromAllOpponents(reciever->Pos(), reciever, power)) {
        closest_to_goal_so_far = std::fabs(reciever->Pos().x - p_AwayGoal->Center().x);
        passTarget = reciever->Pos();
        found = true;
    }

    // directions up, down, left, right,
    // diagonal up left, ...etc
    std::vector<std::pair<double, double>> directions = {
        {receiver_range, 0},
        {0, receiver_range},
        {-receiver_range, 0},
        {0, -receiver_range},
        {receiver_range, -receiver_range},
        {-receiver_range, receiver_range},
        {receiver_range, receiver_range},
        {-receiver_range, -receiver_range},
    };

    for (uint8_t i = 0; i < 8; ++i) {
        Vector direction_vec(directions[i].first, directions[i].second);

        Vector target_norm = VecNormalize(direction_vec);

        // this to make sure the target is within the receiver's range in the
        // diagonal directions.
        Vector target = target_norm * receiver_range + reciever->Pos();

        // this will wrap the target position around the pitch boundaries.
        ValidatePitchBoundaries(target);

        time = p_Pitch->p_Ball->TimeToCoverDistance(passer->Pos(), target, power);

        // ball cannot reach this position.
        if (time < 0.0)
            continue;

        if (IsPassSafeFromAllOpponents(target, reciever, power)) {
            double dist_to_goal = std::fabs(target.x - p_AwayGoal->Center().x);
            found = true;

            if (closest_to_goal_so_far > dist_to_goal) {
                closest_to_goal_so_far = dist_to_goal;
                passTarget = target;
            }
        }
    }

    if (found)
        return true;

    return false;
}
