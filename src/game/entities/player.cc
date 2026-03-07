#include "game/entities/player.h"
#include "game/entities/soccer_ball.h"
#include "game/soccer_team.h"
#include "utils/constants.h"
#include "game/steering_behavs.h"
#include "utils/region.h"
#include "math/vector.h"
#include <cstddef>

Player::Player(SoccerTeam* team, int home_region, Vector _heading)
    : MovingEntity(team->Pitch()->m_Regions[home_region].Center(), constants::PLAYER_RADIUS, Vector(), _heading,
        constants::PLAYER_MAX_SPEED, constants::PLAYER_MAX_FORCE, constants::PLAYER_TURN_RATE, Timer),
        m_CurrentRegion(home_region), m_HomeRegion(home_region) {


    p_SteeringBehaviors = new SteeringBehaviors(this, team->Pitch()->p_Ball, team->Pitch());

    p_SteeringBehaviors->SetTarget(team->Pitch()->m_Regions[home_region].Center());
}

SoccerBall* Player::Ball() const {
    return p_Team->Pitch()->p_Ball;
}

Region Player::HomeRegion() const {
    return p_Team->Pitch()->m_Regions[m_HomeRegion];
}

SoccerPitch* Player::Pitch() const {
    return Team()->Pitch();
}

bool Player::isClosestToBall() const {
    return Team()->ClosestPlayerToBall() == this;
}

bool Player::BallWithinReceivingRange() const {
    return VecDistanceSQ(Pos(), Ball()->Pos()) < constants::BALL_WITHIN_RECEIVING_RANGE_SQ;
}

bool Player::BallWithinKickingRange() const {
    return VecDistanceSQ(Pos(), Ball()->Pos()) < constants::BALL_WITHIN_KICKING_RANGE_SQ;
}

bool Player::isControllingPlayer() const {
    return Team()->ControllingPlayer() == this;
}

bool Player::PositionInFrontOfPlayer(const Vector& pos) const {
    return Heading().Dot(pos - Pos()) > 0.0;
}

bool Player::isThreatened() const {
    Player* opp;
    for(size_t i = 0; i < constants::PLAYERS_PER_TEAM; ++i) {
        opp = Team()->Opponent()->m_Players[i];
        if(PositionInFrontOfPlayer(opp->Pos()) && VecDistanceSQ(Pos(), opp->Pos()) < constants::PLAYER_THREATENING_RANGE_SQ) {
            return true;
        }
    }

    return false;
}

bool Player::AtTarget() const {
    return VecDistanceSQ(Pos(), Steering()->Target()) < constants::PLAYER_IN_TARGET_RANGE_SQ;
}

void Player::TrackBall() {
    RotateHeading(Ball()->Pos());
}
