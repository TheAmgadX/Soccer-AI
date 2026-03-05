#pragma once

#include "../../math/vector.h"
#include "../moving_entity.h"
#include "game/soccer_pitch.h"
#include "soccer_ball.h"
#include "utils/region.h"

class SteeringBehaviors;
class SoccerTeam;


class Player : public MovingEntity {
public:
    enum PlayerRole {
        GOALKEEPER,
        DEFENDER,
        ATTACKER
    };

private:

    SoccerTeam* p_Team;
    SteeringBehaviors* p_SteeringBehaviors;

    int m_CurrentRegion;
    int m_HomeRegion;
    double m_DistanceToBallSQ;
    PlayerRole m_Role;

public:
    Player() : MovingEntity() {}

    Player(SoccerTeam* team, int home_region, PlayerRole role, Vector _heading);

    virtual ~Player() = 0;

    virtual inline void Update() override {}

    virtual inline void HandleMessage(const Message &message) override {}

    inline Vector FuturePos(const double time) const {
        return m_Pos + m_Velocity * time;
    }

    SteeringBehaviors* Steering() { return p_SteeringBehaviors; }

    SoccerTeam* Team() const { return p_Team; }

    void SetDefaultHomeRegion() { m_CurrentRegion = m_HomeRegion; }

    virtual bool InHome() const { return m_CurrentRegion == m_HomeRegion; }

    Region HomeRegion() const;

    SoccerBall* Ball() const;

    SoccerPitch* Pitch() const;

    bool isClosestToBall() const;

    bool BallWithinReceivingRange() const;

    bool BallWithinKickingRange() const;

    bool isControllingPlayer() const;

    bool isThreatened() const;

    bool AtTarget() const;

    void TrackBall();
};
