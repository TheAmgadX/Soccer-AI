#pragma once

#include "../math/vector.h"
#include "../utils/time_system.h"
#include "base_game_entity.h"
#include <algorithm>
#include <cmath>

class MovingEntity : public Entity {

protected:
  Vector m_Velocity;
  Vector m_Heading;
  Vector m_Side;

  double m_MaxSpeed;
  double m_MaxForce;
  double m_MaxTurnRate;

  TimeSystem *timer = Timer;

public:
    MovingEntity()
        : m_Velocity(Vector()), m_Heading(Vector()), m_Side(Vector()),
            m_MaxSpeed(0.0), m_MaxForce(0.0), m_MaxTurnRate(0.0) {}

    MovingEntity(Vector _pos, double _radius, Vector _velocity, Vector _heading,
                double max_speed, double max_force, double turn_rate,
                TimeSystem *_timer)
        : Entity(_pos, _radius), m_Velocity(_velocity), m_Heading(_heading),
            m_Side(_heading.Perp()), m_MaxSpeed(max_speed), m_MaxForce(max_force),
            m_MaxTurnRate(turn_rate), timer(_timer) {}

    virtual ~MovingEntity() = default;

    virtual void Update() = 0;

    Vector Velocity() const { return m_Velocity; }

    void SetVelocity(const Vector &_velocity) {
        m_Velocity = _velocity;
        m_Velocity.Truncate(m_MaxSpeed);
    }

    double Speed() const { return m_Velocity.Length(); }

    double SpeedSQ() const { return m_Velocity.LengthSQ(); }

    Vector Side() const { return m_Side; }

    Vector Heading() const { return m_Heading; }

    void SetHeading(const Vector &_heading) {
        m_Heading = _heading;
        m_Side = m_Heading.Perp();
    }

    void RotateHeading(const Vector &_target) {
        Vector to_target = VecNormalize(_target - m_Pos);

        double desired = atan2(to_target.y, to_target.x);
        double current = atan2(m_Heading.y, m_Heading.x);

        double maxTurn = m_MaxTurnRate * timer->g_TimeElapsed;

        double diff = desired - current;

        while (diff > M_PI)
            diff -= 2.0 * M_PI;

        while (diff < -M_PI)
            diff += 2.0 * M_PI;

        diff = std::clamp(diff, -maxTurn, maxTurn);

        double newAngle = current + diff;

        m_Heading = {std::cos(newAngle), std::sin(newAngle)};

        m_Heading.Normalize();

        m_Side = m_Heading.Perp();
    }

    double MaxSpeed() const { return m_MaxSpeed; }

    double MaxForce() const { return m_MaxForce; }

    double MaxTurnRate() const { return m_MaxTurnRate; }

    void SetMaxSpeed(const double max_speed) {
        m_MaxSpeed = max_speed;
    }

    void SetMaxForce(const double max_force) {
        m_MaxForce = max_force;
    }
};
