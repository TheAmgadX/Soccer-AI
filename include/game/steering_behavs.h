#pragma once

#include <cstdint>

#include "entities/player.h"
#include "entities/soccer_ball.h"
#include "../math/vector.h"
#include "soccer_pitch.h"

#include <vector>

class SteeringBehaviors {
private:
    enum Behaviours {
        none = 0x0000, seek = 0x0001,
        arrive = 0x0002, persuit = 0x0004,
        interpose = 0x0008
    };

    Player* p_Player;
    SoccerBall* p_Ball;
    Vector m_SteeringForce;
    Vector m_Target;
    double m_InterposeDistance;
    SoccerPitch* p_Pitch;

    uint64_t m_Flags;

    std::vector<Vector> m_Antennas;
private:

    Vector Seek();
    Vector Flee(Vector& target);
    Vector Arrive();
    Vector PursuitBall();
    Vector Interpose();
    Vector AvoidPlayers();
    Vector AvoidWalls();


    inline bool On(Behaviours b) const {
        return (m_Flags & b) == b;
    }

    bool AccumulateForce(const Vector& force);

    bool CheckAccumulateForce(const Vector& force);

    Vector SumForces();

    void CreateAntennas(const double length);

    bool DetectIntersection( const Vector& origin, const Vector& antenna,
        const Vector& objFrom, const Vector& objTo, Vector& hitPoint, double& distance);

    void FindIntersectionPlayers(SoccerTeam* team, Vector &closest_intersect_point,
        double &dist_to_closest_piont, double detection_box_len, bool &found);

public:
    SteeringBehaviors(Player* _player, SoccerBall* _ball, SoccerPitch* _pitch) :p_Player(_player), p_Ball(_ball),
        p_Pitch(_pitch), m_SteeringForce(Vector()), m_Target(Vector()), m_InterposeDistance(0.0),
        m_Flags(none) {
        m_Antennas.resize(3);
        CreateAntennas(constants::AntennaLength);
    }

    Vector Calculate();

    void SeekOn() {
        m_Flags |= seek;
    }

    void ArriveOn() {
        m_Flags |= arrive;
    }

    void PersuitOn() {
        m_Flags |= persuit;
    }

    void InterposeOn() {
        m_Flags |= interpose;
    }

    void SeekOff() {
        if(On(seek)) {
            m_Flags ^= seek;
        }
    }

    void ArriveOff() {
        if(On(arrive)) {
            m_Flags ^= arrive;
        }
    }

    void PersuitOff() {
        if(On(persuit)) {
            m_Flags ^= persuit;
        }
    }

    void InterposeOff() {
        if(On(interpose)) {
            m_Flags ^= interpose;
        }
    }

    Vector Target() {
        return m_Target;
    }

    void SetTarget(const Vector& _target) {
        m_Target = _target;
    }

    Vector Force() {
        return m_SteeringForce;
    }

    bool IsSeekOn(){
        return On(seek);
    }

    bool IsArriveOn(){
        return On(arrive);
    }

    bool IsPersuitOn(){
        return On(persuit);
    }

    bool IsInterposeOn(){
        return On(interpose);
    }

};
