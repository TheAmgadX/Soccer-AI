#include "game/soccer_pitch.h"
#include "game/entities/soccer_ball.h"
#include "game/goal.h"
#include "game/soccer_team.h"
#include "state_machine/states/soccer_team_states/prepare_kick_off.h"
#include "utils/constants.h"
#include "utils/region.h"
#include "utils/time_system.h"
#include "utils/wall.h"
#include <cstddef>

SoccerPitch::SoccerPitch() : m_GoalKeeperHasBall(false), m_GameOn(false) {}

void SoccerPitch::Initialize() {
    m_Walls = Wall::CreateWalls();

    Vector ball_pos = Vector((double)constants::FIELD_CENTER_X,
                            (double)constants::FIELD_CENTER_Y);

    p_Ball = new SoccerBall(ball_pos, constants::BALL_RADIUS, m_Walls, Timer);

    Vector blue_right = Vector((double)constants::LEFT_GOAL_X, (double)constants::GOAL_BOTTOM_Y);

    Vector blue_left = Vector((double)constants::LEFT_GOAL_X, (double)constants::GOAL_TOP_Y);

    Vector blue_facing = Vector(1.0, 0.0);

    Vector red_right = Vector((double)constants::RIGHT_GOAL_X, (double)constants::GOAL_TOP_Y);
    Vector red_left = Vector((double)constants::RIGHT_GOAL_X, (double)constants::GOAL_BOTTOM_Y);
    Vector red_facing = Vector(-1.0, 0.0);

    p_BlueGoal = new Goal(blue_right, blue_left, blue_facing);
    p_RedGoal = new Goal(red_right, red_left, red_facing);

    p_BlueTeam = new SoccerTeam(SoccerTeam::TeamColor::BLUE, p_BlueGoal, p_RedGoal, this);
    p_RedTeam = new SoccerTeam(SoccerTeam::TeamColor::RED, p_RedGoal, p_BlueGoal, this);

    CreatePitchRegions();

    p_BlueTeam->Initialize(p_RedTeam);
    p_RedTeam->Initialize(p_BlueTeam);


    p_PlayingArea = new Region(constants::PITCH_TOP, constants::PITCH_BOTTOM,
                                constants::PITCH_LEFT, constants::PITCH_RIGHT, -1);
}

void SoccerPitch::Update() {
    Timer->Update();

    if (m_GameOn) {
        p_Ball->Update();
    }

    p_BlueTeam->Update();
    p_RedTeam->Update();

    if (m_GameOn &&
        (p_RedGoal->CheckGoal(p_Ball->Pos()) || p_BlueGoal->CheckGoal(p_Ball->Pos()))) {
        m_GameOn = false;

        p_Ball->PlaceAtPos(Vector((double)constants::FIELD_CENTER_X,
                                (double)constants::FIELD_CENTER_Y));

        p_RedTeam->FSM()->ChangeState(PrepareForKickOff::Instance());
        p_BlueTeam->FSM()->ChangeState(PrepareForKickOff::Instance());
    }
}

void SoccerPitch::CreatePitchRegions() {
    m_Regions.reserve(constants::NUM_OF_REGIONS_PER_COLUMN * constants::NUM_OF_REGIONS_PER_ROW);

    double region_width = (double)constants::PITCH_WIDTH / constants::NUM_OF_REGIONS_PER_ROW;
    double region_hieght = (double)constants::PITCH_HEIGHT / constants::NUM_OF_REGIONS_PER_COLUMN;

    double x = constants::PITCH_LEFT;
    double y = constants::PITCH_TOP;

    size_t ids = 0;
    for (size_t i = 0; i < constants::NUM_OF_REGIONS_PER_ROW; ++i) {
        for (size_t j = 0; j < constants::NUM_OF_REGIONS_PER_COLUMN; ++j) {
            m_Regions.push_back(
                Region(y, y + region_hieght, x, x + region_width, ids++));
            y += region_hieght;
        }

        y = constants::PITCH_TOP;
        x += region_width;
    }
}

SoccerPitch::~SoccerPitch() {
    delete p_Ball;
    delete p_BlueTeam;
    delete p_RedTeam;
    delete p_BlueGoal;
    delete p_RedGoal;
    delete p_PlayingArea;
}
