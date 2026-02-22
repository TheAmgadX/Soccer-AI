#pragma once
#include "../../utils/constants.h"
#include "../../utils/time_system.h"
#include "../../utils/wall.h"
#include "../moving_entity.h"
#include <vector>

class SoccerBall : public MovingEntity {
  Vector m_OldPos;

  const std::vector<Wall> &m_Walls;

  void CheckCollisionWithWalls();

  void AddNoiseToKick();

public:
  SoccerBall(Vector &_pos, double _radius, const std::vector<Wall> &walls,
             TimeSystem *_timer)
      : MovingEntity(_pos, _radius, Vector(), Vector(),
                     constants::BALL_MAX_SPEED, constants::BALL_MAX_FORCE,
                     constants::BALL_TURN_RATE, _timer),
        m_Walls(walls) {
    m_OldPos = m_Pos;
  }

  void Update() override;

  Vector FuturePos(double time) const;

  inline void HandleMessage(const Message &msg) override {
  } // do nothing, ball doesn't handle messages

  void Kick(Vector &direction, double force);

  double TimeToCoverDistance(const Vector &from, const Vector &to,
                             double force);

  void PlaceAtPos(const Vector &pos);

  inline void Trap() { m_Velocity.Zero(); }

  inline Vector OldPos() const { return m_OldPos; }
};
