#pragma once

namespace constants {

//
// ------------------ Screen & Pitch Dimensions ------------------
//
constexpr int SCREEN_WIDTH = 1300;
constexpr int SCREEN_HEIGHT = 600;

constexpr int PITCH_MARGIN = 50;
constexpr int PITCH_LEFT = PITCH_MARGIN;
constexpr int PITCH_TOP = PITCH_MARGIN;
constexpr int PITCH_WIDTH = SCREEN_WIDTH - (PITCH_MARGIN * 2);
constexpr int PITCH_HEIGHT = SCREEN_HEIGHT - (PITCH_MARGIN * 2);
constexpr int PITCH_RIGHT = PITCH_LEFT + PITCH_WIDTH;
constexpr int PITCH_BOTTOM = PITCH_TOP + PITCH_HEIGHT;

constexpr int FIELD_CENTER_X = PITCH_LEFT + (PITCH_WIDTH / 2);
constexpr int FIELD_CENTER_Y = PITCH_TOP + (PITCH_HEIGHT / 2);


//
// ------------------ Goal Geometry ------------------
//
constexpr int GOAL_DEPTH = 36;
constexpr int GOAL_HEIGHT = 160;

constexpr int GOAL_TOP_Y = FIELD_CENTER_Y - (GOAL_HEIGHT / 2);
constexpr int GOAL_BOTTOM_Y = FIELD_CENTER_Y + (GOAL_HEIGHT / 2);

// Goal-line x values
constexpr int LEFT_GOAL_X = PITCH_LEFT;
constexpr int RIGHT_GOAL_X = PITCH_RIGHT;

// Goal target y values (AI aiming lanes)
constexpr int LEFT_GOAL_LEFT_Y = GOAL_TOP_Y + (GOAL_HEIGHT / 6);
constexpr int LEFT_GOAL_CENTER_Y = FIELD_CENTER_Y;
constexpr int LEFT_GOAL_RIGHT_Y = GOAL_BOTTOM_Y - (GOAL_HEIGHT / 6);

constexpr int RIGHT_GOAL_LEFT_Y = GOAL_TOP_Y + (GOAL_HEIGHT / 6);
constexpr int RIGHT_GOAL_CENTER_Y = FIELD_CENTER_Y;
constexpr int RIGHT_GOAL_RIGHT_Y = GOAL_BOTTOM_Y - (GOAL_HEIGHT / 6);


//
// ------------------ Penalty & Goal Boxes ------------------
//
constexpr int PENALTY_BOX_DEPTH = 180;
constexpr int PENALTY_BOX_HEIGHT = 260;

constexpr int GOAL_BOX_DEPTH = 70;
constexpr int GOAL_BOX_HEIGHT = 140;


//
// ------------------ Game Rules & Team Configuration ------------------
//
constexpr int PLAYERS_PER_TEAM = 5;
constexpr int GOALKEEPER_INDEX = 0;


//
// ------------------ Timing ------------------
//
constexpr int TARGET_FPS = 60;
constexpr int FRAME_DELAY_MS = 1000 / TARGET_FPS;


//
// ------------------ Pitch Rendering ------------------
//
constexpr int PITCH_STRIPE_COUNT = 10;
constexpr int LINE_THICKNESS = 2;

constexpr int CENTER_CIRCLE_RADIUS = 70;
constexpr int CENTER_SPOT_RADIUS = 3;

constexpr int PENALTY_MARK_OFFSET_X = 120;


//
// ------------------ Goal Net Rendering ------------------
//
constexpr int GOAL_NET_INSET = 2;
constexpr int GOAL_NET_SPACING = 8;
constexpr int GOAL_NET_START_OFFSET = 6;


//
// ------------------ Player Visuals ------------------
//
constexpr float PLAYER_SIZE_FIELD = 20.0f;
constexpr float PLAYER_SIZE_GOALKEEPER = 20.0f;

constexpr float PLAYER_WING_ANGLE_OFFSET = 2.52f;

constexpr int PLAYER_OUTLINE_THICKNESS = 2;


//
// ------------------ Ball Visuals ------------------
//
constexpr int BALL_RADIUS = 10;
constexpr int BALL_OUTLINE_THICKNESS = 2;

constexpr int BALL_SHADOW_OFFSET_X = 1;
constexpr int BALL_SHADOW_OFFSET_Y = 1;


//
// ------------------ Default / Demo Positions ------------------
//
constexpr int BALL_START_X = FIELD_CENTER_X;
constexpr int BALL_START_Y = FIELD_CENTER_Y;

constexpr int RED_TEAM_START_X = PITCH_LEFT + 140;
constexpr int BLUE_TEAM_START_X = PITCH_RIGHT - 140;

constexpr int TEAM_START_Y = PITCH_TOP + 80;

constexpr int TEAM_X_SPACING = 90;
constexpr int TEAM_Y_SPACING = 80;


// 
// ------------------ Vector Constants ------------------
//
constexpr double MinDouble = std::numeric_limits<double>::min();

constexpr double Epsilon = std::numeric_limits<double>::epsilon();

} // namespace constants
