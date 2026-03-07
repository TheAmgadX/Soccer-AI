#include "game/soccer_team.h"
#include "game/entities/goal_keeper.h"
#include "game/entities/field_player.h"
#include "game/entities/player.h"
#include "state_machine/message.h"
#include "state_machine/states/soccer_team_states/prepare_kick_off.h"
#include "utils/constants.h"
#include "math/vector.h"
#include "utils/time_system.h"
#include "utils/utils.h"
#include "state_machine/message_manager.h"
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>

SoccerTeam::~SoccerTeam() {
    for(std::size_t i = 0; i < m_Players.size(); ++i) {
        delete m_Players[i];
    }
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

void SoccerTeam::Initialize(SoccerTeam* opponent) {
    m_StateMachine = StateMachine<SoccerTeam>(this, PrepareForKickOff::Instance(), nullptr);
    m_StateMachine.CurrentState()->Enter(this);

    CreatePlayers();
    p_Opponent = opponent;
}

void SoccerTeam::Update() {
    CalcClosestPlayerToBall();

    m_StateMachine.Update();

    if(p_ControllingPlayer) {
        DetermineBestSupportingPlayer();
    }

    for (auto& player : m_Players) {
        player->Update();
    }
}

void SoccerTeam::CalcClosestPlayerToBall() {
    double distanceSQ = MAXFLOAT;
    for(size_t i = 0; i < m_Players.size(); ++i) {
        Vector to_ball = p_Pitch->p_Ball->Pos() - m_Players[i]->Pos();
        double dist = to_ball.LengthSQ();

        if(dist < distanceSQ) {
            distanceSQ = dist;
            p_PlayerClosestToBall = m_Players[i];
        }
    }

    m_DistanceSQToBallFromClosestPlayer = std::sqrt(distanceSQ);
}

void SoccerTeam::DetermineBestSupportingPlayer() {
    Player* best_supporting_player = nullptr;
    double min_distance = MAXFLOAT;

    for (uint8_t i = 1; i < constants::PLAYERS_PER_TEAM; ++i) {
        if(m_Players[i] == p_ControllingPlayer) continue;

        Vector to_spot = GetBestSupportSpot() - m_Players[i]->Pos();
        double distance = to_spot.LengthSQ();

        if(distance < min_distance) {
            min_distance = distance;
            best_supporting_player = m_Players[i];
        }
    }

    p_SupportingPlayer = best_supporting_player;
}

void SoccerTeam::CheckSpot(Spot &spot) const {
    if(IsPassSafeFromAllOpponents(spot.position, nullptr, constants::Max_Passing_Force)) {
        spot.rate += constants::SAFE_PASS_RATE;
    }

    Vector tmp;

    if(CanShoot(spot.position, constants::MAX_SHOT_FORCE, tmp)) {
        spot.rate += constants::CAN_SHOT_RATE;
    }

    // the next test is for distance between controlling player and supporting player.
    if(!p_SupportingPlayer) {
        return;
    }

    Vector to_support_player = p_SupportingPlayer->Pos() - p_ControllingPlayer->Pos();
    double len = to_support_player.LengthSQ();

    if(len > constants::MIN_DISTANCE_TO_SUPPORT_PLAYER && len < constants::MAX_DISTANCE_TO_SUPPORT_PLAYER) {
        spot.rate += constants::DISTANCE_BETWEEN_PLAYERS_RATE;
    }
}

Vector SoccerTeam::GetBestSupportSpot() {
    if(Timer->g_Time - m_TimeSinceLastUpdateToSupportSpot < constants::UPDATE_SUPPORT_SPOT_INTERVAL) {
        return m_BestSupportSpot;
    }

    m_TimeSinceLastUpdateToSupportSpot = Timer->g_Time;

    Spot best_spot;

    for(int i = 0; i < p_Pitch->m_Regions.size(); ++i) {
        Spot spot;
        spot.position = p_Pitch->m_Regions[i].GetRandomPos();
        spot.rate = 1.0;

        CheckSpot(spot);

        if(spot.rate > best_spot.rate) {
            best_spot = spot;
        }
    }

    return best_spot.position;
}

void SoccerTeam::ReturnFieldPlayersToHome() {
    for(size_t i = 1; i < m_Players.size(); ++i) {
        MsgMgr->SendMessage(constants::GAME_ID, m_Players[i]->Id(), MessageEnum::msgGotoHome, constants::SEND_MESSAGE_NOW, nullptr);
    }
}

bool SoccerTeam::AllPlayersAtHome() const {
    for(size_t i = 1; i < constants::PLAYERS_PER_TEAM; ++i) {
        if(!m_Players[i]->InHome()) {
            return false;
        }
    }

    return true;
}

bool SoccerTeam::IsPassSafeFromOpponent(const Vector& target, const Player *const reciever, const Player *const opponent, const double power) const {
    Vector ball_pos = p_Pitch->p_Ball->Pos();
    Vector to_target = target - ball_pos;
    Vector to_target_norm = VecNormalize(to_target);
    Vector perp_to_target = to_target_norm.Perp();

    Vector local_opponent_pos = PointToLocalSpace(opponent->Pos(), to_target_norm, perp_to_target, ball_pos);

    // opponent is behine the ball pass path.
    // this assumes the ball speed is equal to or higher than the opponent's speed.
    if(local_opponent_pos.x < 0) {
        return true;
    }

    double distance_from_opponent_to_target = VecDistanceSQ(opponent->Pos(), target);

    if(VecDistanceSQ(ball_pos, target) < distance_from_opponent_to_target) {
        if(reciever){
            if(distance_from_opponent_to_target > VecDistanceSQ(reciever->Pos(), target)) {
                return true;
            }

            return false;
        } else {
            return true;
        }
    }

    // check if the opponent cannot intersect the ball path.
    double time_to_cover_opponent_x = p_Pitch->p_Ball->TimeToCoverDistance(Vector(0,0), Vector(local_opponent_pos.x, 0), power);

    double distance_opponent_can_cover_in_time = opponent->MaxSpeed() * time_to_cover_opponent_x + p_Pitch->p_Ball->Radius();

    if(distance_opponent_can_cover_in_time > local_opponent_pos.y) {
        return false;
    }

    return true;
}

bool SoccerTeam::IsPassSafeFromAllOpponents(const Vector& target, const Player *const reciever, const double power) const {
    for(size_t i = 0; i < constants::PLAYERS_PER_TEAM; ++i) {
        if(!IsPassSafeFromOpponent(target, reciever, p_Opponent->m_Players[i],power)) {
            return false;
        }
    }

    return true;
}

bool SoccerTeam::CanShoot(const Vector& ball_pos, const double power, Vector& shotTarget) const {
    uint8_t attemps = constants::ATTEMPS_TO_FIND_SHOT_POS;

    while(attemps--) {
        shotTarget = p_AwayGoal->Center();

        double min_y = constants::GOAL_TOP_Y + p_Pitch->p_Ball->Radius();
        double max_y = constants::GOAL_BOTTOM_Y - p_Pitch->p_Ball->Radius();

        shotTarget.y = Utils::RandomInt(min_y, max_y);

        double time = p_Pitch->p_Ball->TimeToCoverDistance(ball_pos, shotTarget, power);

        if(time >= 0) {
            if (IsPassSafeFromAllOpponents(shotTarget, nullptr, power)) {
                return true;
            }
        }
    }

    return false;
}

void SoccerTeam::RequestPass(Player* requester) const {
    // add condition to make the ai not perfect.
    // simulate human behavior.
    if(Utils::RandomFloatClamped() > 0.5) {
        return;
    }

    if(!IsPassSafeFromAllOpponents(requester->Pos(), requester, constants::Max_Passing_Force)) {
        MsgMgr->SendMessage(requester->Id(), p_ControllingPlayer->Id(), MessageEnum::msgPassToMe, constants::SEND_MESSAGE_NOW, requester);
    }
}

bool SoccerTeam::FindPass(const Player* const passer, Player *& receiver,
        Vector & passTarget, const double power, const double minPassDistance) const {
    /*
     * best pass is the pass that cannot be intercepted by the opponent
     * and closest to the opponent goal.
     *
     * Not the smartest logic for football but works fine.
     */

    double closest_to_goal_so_far =  0.0;
    Vector target;


    for(size_t i = 0; i < constants::PLAYERS_PER_TEAM; ++i) {
        if(passer != m_Players[i])
            continue;

        if(VecDistanceSQ(passer->Pos(), m_Players[i]->Pos()) < minPassDistance) {
            continue;
        }

        if(!GetBestPassToReceiver(passer, m_Players[i], target, power)) {
            continue;
        }

        double distance_to_goal = std::fabs(p_AwayGoal->Center().x - target.x);

        if(distance_to_goal < closest_to_goal_so_far) {
            closest_to_goal_so_far = distance_to_goal;
            passTarget = target;
            receiver = m_Players[i];
        }
    }

    // if no pass found.
    if(!receiver) {
        return false;
    }

    return true;
}

bool SoccerTeam::GetBestPassToReceiver(const Player *const passer, const Player *const reciever,
    Vector &passTarget, const double power) const {

    double time = p_Pitch->p_Ball->TimeToCoverDistance(passer->Pos(), reciever->Pos(), power);

    // ball cannot reach the reciever
    if(time < 0.0)
        return false;

    double receiver_range = time * reciever->MaxSpeed();

    double closest_to_goal_so_far = MAXFLOAT;

    bool found = false;

    if(IsPassSafeFromAllOpponents(reciever->Pos(), reciever, power)) {
        closest_to_goal_so_far = std::fabs(reciever->Pos().x - p_AwayGoal->Center().x);
        passTarget = reciever->Pos();
        found = true;
    }

    // directions up, down, left, right,
    // diagonal up left, ...etc
    std::vector<std::pair<double, double>> directions = {
        {receiver_range, 0}, {0, receiver_range}, {-receiver_range, 0}, {0, -receiver_range},
        {receiver_range, -receiver_range}, {-receiver_range, receiver_range},
        {receiver_range, receiver_range}, {-receiver_range, -receiver_range},
    };


    for(uint8_t i = 0; i < 8; ++i) {
        Vector direction_vec(directions[i].first, directions[i].second);

        Vector target_norm = VecNormalize(direction_vec);

        // this to make sure the target is within the receiver's range in the diagonal directions.
        Vector target = target_norm * receiver_range + reciever->Pos();

        // this will wrap the target position around the pitch boundaries.
        ValidatePitchBoundaries(target);

        time = p_Pitch->p_Ball->TimeToCoverDistance(passer->Pos(), target, power);

        // ball cannot reach this position.
        if(time < 0.0)
            continue;

        if(IsPassSafeFromAllOpponents(target, reciever, power)) {
            double dist_to_goal = std::fabs(target.x - p_AwayGoal->Center().x);
            found = true;

            if(closest_to_goal_so_far > dist_to_goal) {
                closest_to_goal_so_far = dist_to_goal;
                passTarget = target;
            }
        }
    }

    if(found)
        return true;

    return false;
}
