#pragma once

#include "../utils/region.h"
#include "../utils/wall.h"
#include "entities/soccer_ball.h"
#include "goal.h"

class SoccerTeam;

class SoccerPitch {
private:
  void CreatePitchRegions();

public:
  SoccerBall *p_Ball;
  SoccerTeam *p_RedTeam;
  SoccerTeam *p_BlueTeam;
  Goal *p_RedGoal;
  Goal *p_BlueGoal;

  Region *p_PlayingArea;
  std::vector<Wall> m_Walls;
  std::vector<Region> m_Regions;

  bool m_GoalKeeperHasBall;
  bool m_GameOn;

public:
  SoccerPitch();
  ~SoccerPitch();

  bool GameOn() const { return m_GameOn; }
  void SetGameOn(bool value) { m_GameOn = value; }

  bool GoalKeeperHasBall() const { return m_GoalKeeperHasBall; }
  void SetGoalKeeperHasBall(bool value) { m_GoalKeeperHasBall = value; }
  void Update();
};
