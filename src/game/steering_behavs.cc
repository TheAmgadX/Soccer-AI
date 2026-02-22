#include "game/steering_behavs.h"
#include "math/vector.h"
#include <algorithm>

bool SteeringBehaviors::AccumulateForce(const Vector &force) {
    double magnitude = m_SteeringForce.Length();

    double remaining_magnitude = p_Player->MaxForce();

    if (remaining_magnitude <= 0.0) {
        return false;
    }

    double force_magnitude = force.Length();

    if (force_magnitude > remaining_magnitude) {
        force_magnitude = remaining_magnitude;
    }

    m_SteeringForce += Vector(VecNormalize(force) * force_magnitude);

    return true;
}

bool SteeringBehaviors::CheckAccumulateForce(const Vector &force) {
    m_SteeringForce += force;

    return AccumulateForce(force);
}

Vector SteeringBehaviors::SumForces() {
    Vector force;

    force = AvoidPlayers();

    if (!CheckAccumulateForce(force)) {
        return m_SteeringForce;
    }

    force += AvoidWalls();

    if (!CheckAccumulateForce(force)) {
        return m_SteeringForce;
    }

    if (On(seek)) {
        force += Seek();

        if (!CheckAccumulateForce(force)) {
            return m_SteeringForce;
        }
    }

    if (On(arrive)) {
        force += Arrive();

        if (!CheckAccumulateForce(force)) {
            return m_SteeringForce;
        }
    }

    if (On(persuit)) {
        force += PursuitBall();

        if (!CheckAccumulateForce(force)) {
            return m_SteeringForce;
        }
    }

    if (On(interpose)) {
        force += Interpose();

        if (!CheckAccumulateForce(force)) {
            return m_SteeringForce;
        }
    }

    return m_SteeringForce;
}

Vector SteeringBehaviors::Calculate() {
    m_SteeringForce.Zero();

    m_SteeringForce = SumForces();

    m_SteeringForce.Truncate(p_Player->MaxForce());

    return m_SteeringForce;
}

Vector SteeringBehaviors::Seek() {
    Vector to_target = m_Target - p_Player->Pos();

    to_target.Normalize();

    Vector velocity = to_target * p_Player->MaxSpeed();

    return velocity - p_Player->Velocity();
}

Vector SteeringBehaviors::Flee(Vector &target) {
    Vector desiredVelocity = p_Player->Pos() - target;

    desiredVelocity.Normalize();

    desiredVelocity = desiredVelocity * p_Player->MaxSpeed();

    return desiredVelocity - p_Player->Velocity();
}

Vector SteeringBehaviors::Arrive() {
    Vector to_target = m_Target - p_Player->Pos();

    double distance = to_target.Length();

    if (distance > 0) {
        double speed = distance / 0.3; // reach fast

        speed = std::min(speed, p_Player->MaxSpeed());

        Vector velocity = to_target * speed / distance;

        return velocity - p_Player->Velocity();
    }

    return Vector();
}

Vector SteeringBehaviors::PursuitBall() {
    Vector to_ball = p_Ball->Pos() - p_Player->Pos();

    double time = 0.0;

    if (p_Ball->Speed() != 0.0) {
        time = to_ball.Length() / p_Ball->Speed();
    }

    m_Target = p_Ball->FuturePos(time);

    return Arrive();
}

Vector SteeringBehaviors::Interpose() {
    Vector target_to_ball = VecNormalize(p_Ball->Pos() - m_Target);

    m_Target += target_to_ball * m_InterposeDistance;

    return Arrive();
}

void SteeringBehaviors::CreateAntennas(const double length) {
    // antenna pointing straight in front
    m_Antennas[0] = p_Player->Pos() + length * p_Player->Heading();

    // antenna to left
    Vector temp = p_Player->Heading();

    VecRotateAroundOrigin(temp, M_PI_2 * 3.5f);
    m_Antennas[1] = p_Player->Pos() + length / 2.0f * temp;

    // antenna to right
    temp = p_Player->Heading();
    VecRotateAroundOrigin(temp, M_PI_2 * 0.5f);
    m_Antennas[2] = p_Player->Pos() + length / 2.0f * temp;
}

bool SteeringBehaviors::DetectIntersection(const Vector &origin,
                                           const Vector &antenna,
                                           const Vector &objFrom,
                                           const Vector &objTo,
                                           Vector &hitPoint, double &distance) {
    Vector antennaEnd = origin + antenna;

    Vector r = antennaEnd - origin;
    Vector s = objTo - objFrom;

    double rxs = r.x * s.y - r.y * s.x;
    double qpxr = (objFrom - origin).x * r.y - (objFrom - origin).y * r.x;

    if (std::fabs(rxs) < 0.000001)
        return false;

    double t = ((objFrom - origin).x * s.y - (objFrom - origin).y * s.x) / rxs;
    double u = qpxr / rxs;

    if (t >= 0.0 && t <= 1.0 && u >= 0.0 && u <= 1.0) {
        hitPoint = origin + r * t;
        distance = antenna.Length() * t;
        return true;
    }

    return false;
}

Vector SteeringBehaviors::AvoidWalls() {
    double distance_to_ip = 0.0;
    double distance_to_closest_ip = MAXFLOAT;
    int closest_wall = -1;

    Vector force, point, closest_point;

    for (size_t ant = 0; ant < 3; ++ant) {
        for (size_t i = 0; i < 4; ++i) {
        if (DetectIntersection(
                p_Player->Pos(), m_Antennas[ant], p_Pitch->m_Walls[i].From(),
                p_Pitch->m_Walls[i].To(), point, distance_to_ip)) {

            if (distance_to_closest_ip > distance_to_ip) {
            closest_point = point;
            distance_to_closest_ip = distance_to_ip;
            closest_wall = i;
            }
        }
        }

        if (closest_wall != -1) {
        // how much the antenna passed the wall.
        Vector over_shoot = m_Antennas[ant] - closest_point;

        // create a force in the direction of wall normal with a magnitude of
        // over_shoot
        force = p_Pitch->m_Walls[closest_wall].Normal() * over_shoot.Length();

        closest_wall = -1;
        }
    }

    return force;
}

void SteeringBehaviors::FindIntersectionPlayers(SoccerTeam *team,
                                                Vector &closest_intersect_point,
                                                double &dist_to_closest_piont,
                                                double detection_box_len,
                                                bool &found) {

    Vector detection_box_point =
        p_Player->Pos() + (p_Player->Heading() * detection_box_len);

    // temp variables used in the loop.
    double distance_to_ip = -1;
    Vector point;

    for (size_t i = 0; i < constants::PLAYERS_PER_TEAM; ++i) {
        if (p_Player == team->m_Players[i])
        continue;
        Player *player = team->m_Players[i];

        Vector player_future_pos =
            player->Pos() + (player->Heading() * detection_box_len);

        if (DetectIntersection(p_Player->Pos(), detection_box_point, player->Pos(),
                            player_future_pos, point, distance_to_ip)) {

        if (dist_to_closest_piont > distance_to_ip) {
            closest_intersect_point = point;
            dist_to_closest_piont = distance_to_ip;
            found = true;
        }
        }
    }
}

Vector SteeringBehaviors::AvoidPlayers() {
    // if the player not moving, do nothing.
    if (p_Player->Speed() == 0)
        return Vector();

    Vector closest_intersect_point;
    bool found = false;
    double dist_to_closest_piont = MAXFLOAT;

    double detection_box_len = constants::MinDetectionBoxLength +
                                (p_Player->Speed() / p_Player->MaxSpeed() *
                                constants::MinDetectionBoxLength);

    FindIntersectionPlayers(p_Pitch->p_RedTeam, closest_intersect_point,
                            dist_to_closest_piont, detection_box_len, found);
    FindIntersectionPlayers(p_Pitch->p_BlueTeam, closest_intersect_point,
                            dist_to_closest_piont, detection_box_len, found);

    Vector force;

    // if there is yes intersection players.
    if (!found)
        return force;

    force = Flee(closest_intersect_point);

    force.Normalize();

    dist_to_closest_piont =
        std::clamp(dist_to_closest_piont, 0.5, detection_box_len);

    return force * (1.0 + 1.0 / dist_to_closest_piont);
}
