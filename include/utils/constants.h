#pragma once

#include <limits>
#include <cstdint>

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

constexpr double PLAYER_RADIUS = 20.0f;

constexpr float PLAYER_TURN_RATE = 1.0f; // TODO: set proper value for this

//
// ------------------ Ball Visuals ------------------
//
constexpr int BALL_RADIUS = 10;
constexpr int BALL_OUTLINE_THICKNESS = 2;

constexpr int BALL_SHADOW_OFFSET_X = 1;
constexpr int BALL_SHADOW_OFFSET_Y = 1;

constexpr double BALL_MAX_SPEED = 10.0;
constexpr double BALL_MAX_FORCE = 100.0;
constexpr double BALL_TURN_RATE = 1.0;

//
// ------------------ Default / Demo Positions ------------------
//
constexpr int BALL_START_X = FIELD_CENTER_X;
constexpr int BALL_START_Y = FIELD_CENTER_Y;

constexpr int RED_TEAM_START_X =  PITCH_RIGHT + 140;
constexpr int BLUE_TEAM_START_X = PITCH_LEFT - 140;

constexpr int TEAM_START_Y = PITCH_TOP + 80;

constexpr int TEAM_X_SPACING = 90;
constexpr int TEAM_Y_SPACING = 80;


//
// ------------------ Vector Constants ------------------
//
constexpr double MinDouble = std::numeric_limits<double>::min();

constexpr double Epsilon = std::numeric_limits<double>::epsilon();

constexpr double FRICTION = -0.5; // TODO: assign proper value for this.

constexpr double AntennaLength = 4.0; // TODO: assign proper value for this.

constexpr double MinDetectionBoxLength = 5.0; // TODO: assign proper value for this.

//
// ------------------ Game AI Constants ------------------
//

constexpr double KickAccuracy = 0.9;

constexpr double Restitution = 0.8;

constexpr uint8_t ATTEMPS_TO_FIND_SHOT_POS = 5;

constexpr double UPDATE_SUPPORT_SPOT_INTERVAL = 0.5; // TODO: assign proper value for this.

constexpr double MAX_DISTANCE_TO_SUPPORT_PLAYER = 20.0; // TODO: assign proper value for this.

constexpr double MIN_DISTANCE_TO_SUPPORT_PLAYER = 10.0; // TODO: assign proper value for this.

constexpr double MIN_PASS_DISTANCE = 5.0; // TODO: assign proper value for this.

constexpr double Max_Passing_Force = 100.0; // TODO: assign proper value for this.

constexpr double MAX_DRIBBLE_FORCE = 0.8; // TODO: assign proper value for this.

constexpr double MAX_SHOT_FORCE = 150.0; // TODO: assign proper value for this.

constexpr double PLAYER_MAX_SPEED = 100.0; // TODO: assign proper value for this.

constexpr double PLAYER_MAX_SPEED_WITH_BALL = 80.0; // TODO: assign proper value for this.

constexpr double PLAYER_MAX_FORCE = 100.0; // TODO: assign proper value for this.

constexpr double PLAYER_IN_TARGET_RANGE_SQ = 4.0 * 4.0; // TODO: assign proper value for this.

constexpr double BALL_WITHIN_RECEIVING_RANGE_SQ = 2.0 * 2.0; // TODO: assign proper value for this.

constexpr double BALL_WITHIN_KICKING_RANGE_SQ = 3.0 * 3.0; // TODO: assign proper value for this.

constexpr double PLAYER_THREATENING_RANGE_SQ = 5.0 * 5.0; // TODO: assign proper value for this.

constexpr double PROTECT_GOAL_DISTANCE = 10.0; // TODO: assign proper value for this.

constexpr double KEEPER_RANGE_SQ = 225.0; // TODO: assign proper value for this.

constexpr double INTERCEPT_RANGE_SQ = 400.0; // TODO: assign proper value for this.

// --- constants for determining the best supporting spot.
constexpr double SAFE_PASS_RATE = 2.0;

constexpr double CAN_SHOT_RATE = 4.0;

constexpr double DISTANCE_BETWEEN_PLAYERS_RATE = 1.0;

constexpr int NUM_OF_REGIONS_PER_ROW = 6;
constexpr int NUM_OF_REGIONS_PER_COLUMN = 3;

//
// ----------------- Messaging Constants ------------------
//

constexpr int SEND_MESSAGE_NOW = -1;
constexpr int GAME_ID = 0;


} // namespace constants
