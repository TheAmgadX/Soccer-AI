#include "game/entities/goal_keeper.h"
#include "game/entities/player.h"
#include "game/soccer_pitch.h"
#include "game/soccer_team.h"
#include "game/steering_behavs.h"
#include "math/vector.h"
#include "state_machine/message.h"
#include "state_machine/message_manager.h"
#include "utils/constants.h"

#include "state_machine/states/goal_keeper_states/global.h"
#include "state_machine/states/goal_keeper_states/intercept.h"
#include "state_machine/states/goal_keeper_states/protect_goal.h"
#include "state_machine/states/goal_keeper_states/put_ball_back.h"
#include "state_machine/states/goal_keeper_states/return_home.h"

namespace {
bool BallInKeeperPriorityZone(const GoalKeeper *entity) {
  const Vector ball_pos = entity->Ball()->Pos();

  const double top =
      constants::FIELD_CENTER_Y - (constants::PENALTY_BOX_HEIGHT * 0.5);
  const double bottom =
      constants::FIELD_CENTER_Y + (constants::PENALTY_BOX_HEIGHT * 0.5);
  if (ball_pos.y < top || ball_pos.y > bottom) {
    return false;
  }

  if (entity->Team()->Color() == SoccerTeam::BLUE) {
    return ball_pos.x <= constants::PITCH_LEFT + constants::PENALTY_BOX_DEPTH;
  }

  return ball_pos.x >= constants::PITCH_RIGHT - constants::PENALTY_BOX_DEPTH;
}
} // namespace

// ----------------------------------------
// ---------------- Global ----------------
// ----------------------------------------
void GoalKeeperGlobal::Enter(GoalKeeper *entity) {}

void GoalKeeperGlobal::Process(GoalKeeper *entity) {}

void GoalKeeperGlobal::Exit(GoalKeeper *entity) {}

bool GoalKeeperGlobal::onMessage(GoalKeeper *entity, const Message &msg) {
  switch (msg.msg) {
  case MessageEnum::msgNULL:
    return false;
  case MessageEnum::msgReceiveBall:
    if (!entity->Team()->InControl() && BallInKeeperPriorityZone(entity)) {
      entity->FSM()->ChangeState(GoalKeeperIntercept::Instance());
    }
    return true;
  case MessageEnum::msgGotoHome:
    entity->FSM()->ChangeState(GoalKeeperReturnHome::Instance());
    return true;
  default:
    return false;
  }
}

// --------------------------------------
// -------------- Intercept -------------
// --------------------------------------
void GoalKeeperIntercept::Enter(GoalKeeper *entity) {
  entity->Steering()->PersuitOn();
}

void GoalKeeperIntercept::Process(GoalKeeper *entity) {
  if (!entity->isClosestToBall() || !BallInKeeperPriorityZone(entity)) {
    entity->FSM()->ChangeState(GoalKeeperReturnHome::Instance());
    return;
  }

  if (entity->BallWithinInterceptRange()) {
    entity->Ball()->Trap();
    entity->Pitch()->SetGoalKeeperHasBall(true);
    entity->FSM()->ChangeState(GoalKeeperPutBallBackInPlay::Instance());
    return;
  }
}

void GoalKeeperIntercept::Exit(GoalKeeper *entity) {
  entity->Steering()->PersuitOff();
}

// -----------------------------------------
// -------------- Protect Goal -------------
// -----------------------------------------
void GoalKeeperProtectGoal::Enter(GoalKeeper *entity) {
  entity->Steering()->InterposeOn();

  entity->Steering()->SetTarget(entity->GetRearInterposeTarget());
}

void GoalKeeperProtectGoal::Process(GoalKeeper *entity) {
  entity->Steering()->SetTarget(entity->GetRearInterposeTarget());
  if (entity->Team()->InControl()) {
    entity->FSM()->ChangeState(GoalKeeperReturnHome::Instance());
  }

  if (entity->BallWithinKeeperRange()) {
    entity->Ball()->Trap();
    entity->Pitch()->SetGoalKeeperHasBall(true);
    entity->FSM()->ChangeState(GoalKeeperPutBallBackInPlay::Instance());
    return;
  }

  if (!entity->Team()->InControl() && entity->isClosestToBall() &&
      BallInKeeperPriorityZone(entity)) {
    entity->FSM()->ChangeState(GoalKeeperIntercept::Instance());
    return;
  }
}

void GoalKeeperProtectGoal::Exit(GoalKeeper *entity) {
  entity->Steering()->InterposeOff();
}

// --------------------------------------------------
// -------------- Put Ball Back In Play -------------
// --------------------------------------------------
void GoalKeeperPutBallBackInPlay::Enter(GoalKeeper *entity) {
  entity->Team()->setControllingPlayer(entity);

  // return all players to their home positions.
  entity->Team()->Opponent()->ReturnFieldPlayersToHome();
  entity->Team()->ReturnFieldPlayersToHome();

  entity->SetVelocity(Vector());
}

void GoalKeeperPutBallBackInPlay::Process(GoalKeeper *entity) {
  Player *receiver = nullptr;
  Vector ball_target;

  if (entity->Team()->FindPass(entity, receiver, ball_target,
                               constants::Max_Passing_Force,
                               constants::MIN_PASS_DISTANCE)) {
    Vector pass_direction = ball_target - entity->Ball()->Pos();
    entity->Ball()->Kick(pass_direction, constants::Max_Passing_Force);
    entity->Pitch()->SetGoalKeeperHasBall(false);

    Vector *pass_location = new Vector(
        ball_target); // TODO: this is not deleted anywhere, you should handle
                      // it later after finishing the project.

    MsgMgr->SendMessage(entity->Id(), receiver->Id(),
                        MessageEnum::msgReceiveBall,
                        constants::SEND_MESSAGE_NOW, pass_location);

    entity->Team()->setControllingPlayer(nullptr);
    entity->FSM()->ChangeState(GoalKeeperProtectGoal::Instance());
    return;
  }

  // Fallback: clear long to avoid deadlock when no pass is available.
  Vector clear_direction =
      entity->Team()->AwayGoal()->Center() - entity->Ball()->Pos();
  entity->Ball()->Kick(clear_direction, constants::MAX_SHOT_FORCE);
  entity->Pitch()->SetGoalKeeperHasBall(false);
  entity->Team()->setControllingPlayer(nullptr);
  entity->FSM()->ChangeState(GoalKeeperProtectGoal::Instance());
}

void GoalKeeperPutBallBackInPlay::Exit(GoalKeeper *entity) {}

// --------------------------------------
// -------------- Retrun Home -------------
// --------------------------------------
void GoalKeeperReturnHome::Enter(GoalKeeper *entity) {
  entity->Steering()->ArriveOn();
  entity->Steering()->SetTarget(entity->HomeRegion().Center());
}

void GoalKeeperReturnHome::Process(GoalKeeper *entity) {
  if (entity->InHome()) {
    entity->FSM()->ChangeState(GoalKeeperProtectGoal::Instance());
  }
}

void GoalKeeperReturnHome::Exit(GoalKeeper *entity) {
  entity->Steering()->ArriveOff();
}
