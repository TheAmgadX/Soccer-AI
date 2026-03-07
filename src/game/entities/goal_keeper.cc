#include "game/entities/goal_keeper.h"
#include "math/vector.h"
#include "state_machine/state_machine.h"
#include "state_machine/states/goal_keeper_states/global.h"
#include "state_machine/states/goal_keeper_states/return_home.h"
#include "game/steering_behavs.h"
#include "game/soccer_team.h"

#include <algorithm>

GoalKeeper::GoalKeeper(SoccerTeam* team, int home_region, Vector _heading) : Player(team, home_region, _heading) {
    p_StateMachine = new StateMachine<GoalKeeper>(this, GoalKeeperReturnHome::Instance(), GoalKeeperGlobal::Instance());
    p_StateMachine->CurrentState()->Enter(this);
}

GoalKeeper::~GoalKeeper() {
    delete p_StateMachine;
}

void GoalKeeper::Update() {
    p_StateMachine->Update();

    Vector steering_force = Steering()->Calculate();

    m_Velocity += steering_force * Timer->g_TimeElapsed;

    m_Velocity.Truncate(m_MaxSpeed);

    m_Pos += m_Velocity * Timer->g_TimeElapsed;
    ValidatePitchBoundaries(m_Pos);

    if(!m_Velocity.isZero()) {
        m_Heading = VecNormalize(m_Velocity);

        m_Side = m_Heading.Perp();
    }

    if(!Pitch()->GoalKeeperHasBall()) {
        RotateHeading(Ball()->Pos());
    }
}

void GoalKeeper::HandleMessage(const Message &message) {
    if (p_StateMachine->GlobalState() &&
        p_StateMachine->GlobalState()->onMessage(this, message)) {
        return;
    }

    if (p_StateMachine->CurrentState()) {
        p_StateMachine->CurrentState()->onMessage(this, message);
    }
}

Vector GoalKeeper::GetRearInterposeTarget() const {
    double x;
    switch (Team()->Color()) {
        case SoccerTeam::RED:
            x = Team()->HomeGoal()->Center().x - constants::PROTECT_GOAL_DISTANCE;
            break;
        case SoccerTeam::BLUE:
            x = Team()->HomeGoal()->Center().x + constants::PROTECT_GOAL_DISTANCE;
            break;
    }

    double y = std::clamp(Ball()->Pos().y, constants::GOAL_BOTTOM_Y + 20.0, constants::GOAL_TOP_Y + 20.0);

    return Vector(x, y);
}

bool GoalKeeper::BallWithinKeeperRange() const {
    return VecDistanceSQ(Ball()->Pos(), m_Pos) <= constants::KEEPER_RANGE_SQ;
}

bool GoalKeeper::BallWithinInterceptRange() const {
    return VecDistanceSQ(Ball()->Pos(), m_Pos) <= constants::INTERCEPT_RANGE_SQ;
}
