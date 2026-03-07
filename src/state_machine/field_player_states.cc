#include "game/entities/field_player.h"
#include "game/soccer_team.h"
#include "game/steering_behavs.h"
#include "math/vector.h"
#include "state_machine/message.h"
#include "state_machine/message_manager.h"
#include "utils/constants.h"

#include "state_machine/states/field_player_states/chase.h"
#include "state_machine/states/field_player_states/dribble.h"
#include "state_machine/states/field_player_states/global.h"
#include "state_machine/states/field_player_states/kick.h"
#include "state_machine/states/field_player_states/receive.h"
#include "state_machine/states/field_player_states/return_to_home.h"
#include "state_machine/states/field_player_states/support.h"
#include "state_machine/states/field_player_states/wait.h"
#include "utils/utils.h"
#include <algorithm>

// ---------------------------
// ---------- Chase ----------
// ---------------------------
void FieldPlayerChase::Enter(FieldPlayer *entity)
{
  entity->Steering()->SeekOn();
}

void FieldPlayerChase::Exit(FieldPlayer *entity)
{
  entity->Steering()->SeekOff();
}

void FieldPlayerChase::Process(FieldPlayer *entity)
{
  if (entity->BallWithinKickingRange())
  {
    entity->FSM()->ChangeState(FieldPlayerKick::Instance());
    return;
  }

  if (entity->isClosestToBall())
  {
    entity->Steering()->SetTarget(entity->Ball()->Pos());
    return;
  }

  entity->FSM()->ChangeState(FieldPlayerReturnToHome::Instance());
}

// ----------------------------
// --------- Dribble ----------
// ----------------------------
void FieldPlayerDribble::Enter(FieldPlayer *entity)
{
  entity->Team()->setControllingPlayer(entity);
}

void FieldPlayerDribble::Exit(FieldPlayer *entity) {}

void FieldPlayerDribble::Process(FieldPlayer *entity)
{
    Vector direction = entity->Team()->AwayGoal()->Center() - entity->Ball()->Pos();

    if (direction.LengthSQ() <= constants::Epsilon) {
        direction = entity->Team()->AwayGoal()->Center() - entity->Pos();
    }

    entity->Ball()->Kick(direction, constants::MAX_DRIBBLE_FORCE);
    entity->FSM()->ChangeState(FieldPlayerChase::Instance());
}

// ---------------------------
// --------- Global ----------
// ---------------------------
void FieldPlayerGlobal::Enter(FieldPlayer *entity) {}

void FieldPlayerGlobal::Exit(FieldPlayer *entity) {}

void FieldPlayerGlobal::Process(FieldPlayer *entity)
{
  if (entity->BallWithinReceivingRange() && entity->isControllingPlayer())
  {
    entity->SetMaxSpeed(constants::PLAYER_MAX_SPEED_WITH_BALL);
  }
  else
  {
    entity->SetMaxSpeed(constants::PLAYER_MAX_SPEED);
  }
}

bool FieldPlayerGlobal::onMessage(FieldPlayer *entity, const Message &msg)
{
  switch (msg.msg)
  {
  case msgReceiveBall:
  {
    (void)msg;
    entity->Steering()->SetTarget(entity->Ball()->Pos());

    entity->FSM()->ChangeState(FieldPlayerReceive::Instance());

    return true;
  }

  case msgPassToMe:
  {
    FieldPlayer *receiver = static_cast<FieldPlayer *>(msg.extraInfo);

    // if the ball cannot be kicked or there is already another receiving
    // player.
    if (!entity->BallWithinKickingRange() ||
        entity->Team()->ReceivingPlayer() != nullptr)
    {
      return true;
    }

    Vector to_receiver = receiver->Pos() - entity->Ball()->Pos();

    entity->Ball()->Kick(to_receiver, constants::Max_Passing_Force);

    Vector *tmp = new Vector(
        receiver->Pos()); // TODO: this may cause memory leak, handle it when
                          // you finish working on the whole project.

    MsgMgr->SendMessage(entity->Id(), receiver->Id(),
                        MessageEnum::msgReceiveBall,
                        constants::SEND_MESSAGE_NOW, tmp);

    entity->FSM()->ChangeState(FieldPlayerWait::Instance());

    return true;
  }

  case msgGotoHome:
  {
    entity->SetDefaultHomeRegion();

    entity->FSM()->ChangeState(FieldPlayerReturnToHome::Instance());

    return true;
  }

  case msgSupport:
  {
    if (entity->FSM()->IsInState(FieldPlayerSupport::Instance()))
    {
      return true;
    }

    // best supporting spot is calculated in soccer team state process method.
    entity->Steering()->SetTarget(entity->Team()->BestSupportingSpot());

    entity->FSM()->ChangeState(FieldPlayerSupport::Instance());

    return true;
  }

  case msgWait:
  {
    entity->FSM()->ChangeState(FieldPlayerWait::Instance());
    return true;
  }

  case msgNULL:
  {
    break;
  }
  }

  return false;
}

// ---------------------------
// ---------- Kick -----------
// ---------------------------
void FieldPlayerKick::Enter(FieldPlayer *entity)
{
  entity->Team()->SetReceivingPlayer(nullptr);
  entity->Team()->setControllingPlayer(entity);
}

void FieldPlayerKick::Exit(FieldPlayer *entity) {}

void FieldPlayerKick::Process(FieldPlayer *entity)
{
  Vector to_ball = entity->Ball()->Pos() - entity->Pos();
  double dot = entity->Heading().Dot(VecNormalize(to_ball));

  // if ball is behind the player, or the goal keeper has the ball
  if (entity->Pitch()->GoalKeeperHasBall() || dot < 0)
  {
    entity->FSM()->ChangeState(FieldPlayerChase::Instance());
    return;
  }

  Vector ball_target;

  double power = std::max(constants::MAX_SHOT_FORCE * dot,
                          constants::MAX_SHOT_FORCE * 0.55);
  Vector ball_pos = entity->Ball()->Pos();
  bool should_shoot = entity->Team()->CanShoot(ball_pos, power, ball_target);

  // Encourage attempts even when no strictly "safe" shot lane is found.
  if (!should_shoot && Utils::Range(0.0, 1.0) < 0.20)
  {
    ball_target = entity->Team()->AwayGoal()->Center();
    should_shoot = true;
  }

  if (should_shoot)
  {
    Vector shot_direction = ball_target - entity->Ball()->Pos();
    entity->Ball()->Kick(shot_direction, power);
    entity->FSM()->ChangeState(FieldPlayerWait::Instance());

    return;
  }

  Player *receiver = nullptr;

  if (entity->isThreatened() &&
      entity->Team()->FindPass(entity, receiver, ball_target, power,
                               constants::MIN_PASS_DISTANCE))
  {
    Vector pass_direction = ball_target - entity->Ball()->Pos();

    entity->Ball()->Kick(pass_direction, power);

    Vector *ptr =
        new Vector(ball_target); // send ball_target position (not direction) so
                                 // receiver runs to the correct spot.
    MsgMgr->SendMessage(entity->Id(), receiver->Id(),
                        MessageEnum::msgReceiveBall,
                        constants::SEND_MESSAGE_NOW, ptr);

    entity->FSM()->ChangeState(FieldPlayerWait::Instance());

    return;
  }

  entity->FSM()->ChangeState(FieldPlayerDribble::Instance());
}

// ------------------------------
// ---------- Receive -----------
// ------------------------------
void FieldPlayerReceive::Enter(FieldPlayer *entity)
{
  entity->Team()->SetReceivingPlayer(entity);

  entity->Steering()->SeekOn();
  entity->Steering()->SetTarget(entity->Ball()->Pos());
}

void FieldPlayerReceive::Exit(FieldPlayer *entity)
{
  entity->Team()->SetReceivingPlayer(nullptr);

  entity->Steering()->SeekOff();
}

void FieldPlayerReceive::Process(FieldPlayer *entity)
{
  entity->Steering()->SetTarget(entity->Ball()->Pos());

  if (entity->BallWithinReceivingRange())
  {
    entity->FSM()->ChangeState(FieldPlayerChase::Instance());
  }
}

// -------------------------------------
// ---------- Return To Home -----------
// -------------------------------------
void FieldPlayerReturnToHome::Enter(FieldPlayer *entity)
{
  entity->Steering()->ArriveOn();

  if (!entity->InHome())
  {
    entity->Steering()->SetTarget(entity->HomeRegion().Center());
  }
}

void FieldPlayerReturnToHome::Exit(FieldPlayer *entity)
{
  entity->Steering()->ArriveOff();
}

void FieldPlayerReturnToHome::Process(FieldPlayer *entity)
{
  if (!entity->Pitch()->GameOn())
  {
    if (entity->InHome())
      entity->FSM()->ChangeState(FieldPlayerWait::Instance());

    return;
  }

  if (entity->isClosestToBall() &&
      entity->Team()->ReceivingPlayer() == nullptr &&
      !entity->Pitch()->GoalKeeperHasBall())
  {

    entity->FSM()->ChangeState(FieldPlayerChase::Instance());
    return;
  }

  if (entity->InHome())
    entity->FSM()->ChangeState(FieldPlayerWait::Instance());
}

// ------------------------------
// ---------- Support -----------
// ------------------------------
void FieldPlayerSupport::Enter(FieldPlayer *entity)
{
  entity->Steering()->ArriveOn();
  entity->Steering()->SetTarget(entity->Team()->SupportSpotFor(entity));
}

void FieldPlayerSupport::Exit(FieldPlayer *entity)
{
  entity->Steering()->ArriveOff();
  entity->Team()->ClearSupportingPlayer(entity);
}

void FieldPlayerSupport::Process(FieldPlayer *entity)
{
  if (!entity->Team()->InControl())
  {
    entity->FSM()->ChangeState(FieldPlayerReturnToHome::Instance());
    return;
  }

  if (entity->Team()->SupportSpotFor(entity) != entity->Steering()->Target())
  {
    entity->Steering()->SetTarget(entity->Team()->SupportSpotFor(entity));
    entity->Steering()->ArriveOn();
  }

  Vector temp;

  if (!entity->isThreatened())
  {
    entity->Team()->RequestPass(entity);
  }

  if (entity->AtTarget())
  {
    entity->Steering()->ArriveOff();

    entity->TrackBall();

    entity->SetVelocity(Vector());
  }
}

// ---------------------------
// ---------- Wait -----------
// ---------------------------
void FieldPlayerWait::Enter(FieldPlayer *entity) {}

void FieldPlayerWait::Exit(FieldPlayer *entity) {}

void FieldPlayerWait::Process(FieldPlayer *entity)
{
  entity->TrackBall();

  if (entity->Pitch()->GameOn())
  {
    if (entity->isClosestToBall() &&
        entity->Team()->ReceivingPlayer() == nullptr &&
        !entity->Pitch()->GoalKeeperHasBall())
    {
      entity->FSM()->ChangeState(FieldPlayerChase::Instance());
    }
  }
}
