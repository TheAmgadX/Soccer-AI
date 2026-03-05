#include "game/entities/player.h"
#include "game/entities/soccer_ball.h"
#include "game/soccer_team.h"
#include "utils/constants.h"
#include "game/steering_behavs.h"
#include "utils/region.h"

Player::~Player() {

}

Player::Player(SoccerTeam* team, int home_region, PlayerRole role, Vector _heading)
    : MovingEntity(team->Pitch()->m_Regions[home_region].Center(), constants::PLAYER_RADIUS, Vector(), _heading,
        constants::PLAYER_MAX_SPEED, constants::PLAYER_MAX_FORCE, constants::PLAYER_TURN_RATE, Timer),
        m_CurrentRegion(home_region), m_HomeRegion(home_region), m_Role(role) {


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
