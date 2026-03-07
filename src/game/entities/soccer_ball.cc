#include "game/entities/soccer_ball.h"
#include "math/vector.h"
#include "utils/constants.h"
#include "utils/time_system.h"
#include "utils/utils.h"
#include <cmath>
#include <cstdlib>

void SoccerBall::Kick(Vector &direction, double force) {
  direction.Normalize();
  Vector acceleration = direction * force;
  m_Velocity = acceleration;

  AddNoiseToKick();
}

void SoccerBall::AddNoiseToKick() {
  double displacement =
      (M_PI - M_PI * constants::KickAccuracy) * Utils::RandomFloatClamped();

  VecRotateAroundOrigin(m_Velocity, displacement);
}

Vector SoccerBall::FuturePos(double time) const {
  /*
          x = ut + 0.5 + (a * t^2)
          x -> distance
          a -> friction
          u -> start velocity
  */

  Vector ut = m_Velocity * time;

  double half_friction_timeSQ = 0.5 * constants::FRICTION * time * time;

  return m_Pos + ut + (VecNormalize(m_Velocity) * half_friction_timeSQ);
}

void SoccerBall::Update() {
  m_OldPos = m_Pos;

  CheckCollisionWithWalls();

  if (m_Velocity.LengthSQ() > constants::FRICTION * constants::FRICTION) {
    m_Velocity += VecNormalize(m_Velocity) * constants::FRICTION;

    m_Pos += m_Velocity * timer->g_TimeElapsed;

    m_Heading = VecNormalize(m_Velocity);
  }
}

void SoccerBall::PlaceAtPos(const Vector &pos) {
  m_Pos = pos;
  m_OldPos = pos;
  m_Velocity = Vector(0, 0);
}

void SoccerBall::CheckCollisionWithWalls() {
  for (const Wall &wall : m_Walls) {
    // Do not bounce on the side goal-mouth opening, so the ball can cross
    // the goal line and be detected as a goal.
    const bool is_vertical_wall = std::fabs(wall.From().x - wall.To().x) < constants::Epsilon;
    if (is_vertical_wall) {
      const double wall_x = wall.From().x;
      const bool is_side_goal_line =
          std::fabs(wall_x - constants::LEFT_GOAL_X) < constants::Epsilon ||
          std::fabs(wall_x - constants::RIGHT_GOAL_X) < constants::Epsilon;

      if (is_side_goal_line &&
          m_Pos.y >= constants::GOAL_TOP_Y - m_Radius &&
          m_Pos.y <= constants::GOAL_BOTTOM_Y + m_Radius) {
        continue;
      }
    }

    // based on vector projection.
    Vector wall_to_ball = m_Pos - wall.From();

    double distance = wall_to_ball.Dot(wall.Normal());

    // if the ball touched the wall
    // NOTE: 0.01 is because of floating point precision errors.
    if (distance <= m_Radius + 0.01) {

      // make sure the velocity is in direction of the wall.
      if (m_Velocity.Dot(wall.Normal()) < 0.0) {
        m_Velocity.Reflect(wall.Normal());

        m_Velocity *= constants::Restitution;

        m_Heading = VecNormalize(m_Velocity);
      }
    }
  }
}

double SoccerBall::TimeToCoverDistance(const Vector &from, const Vector &to,
                                       double force) {
  // we assume the ball mass is 1 so:
  // velocity = force / mass ---> which is equal force

  // Equations used:
  // v^2 = u^2 + 2ad
  // v = u + at ---> t = (v - u) / a

  Vector direction = to - from;
  double distance = direction.Length();
  double vSQ = force * force + 2.0 * constants::FRICTION * distance;

  // will not reach
  if (vSQ <= 0.0) {
    return -1.0;
  }

  double v = std::sqrt(vSQ);

  return (v - force) / constants::FRICTION;
}
