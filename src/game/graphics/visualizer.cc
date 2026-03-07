#include "game/graphics/visualizer.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cmath>

#include "game/entities/player.h"
#include "game/soccer_pitch.h"
#include "game/soccer_team.h"
#include "utils/constants.h"

using namespace constants;

bool SoccerVisualizer::Init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    return false;
  }

  window_ = SDL_CreateWindow("Pixel Soccer AI", SDL_WINDOWPOS_CENTERED,
                             SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                             SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

  return window_ != nullptr && renderer_ != nullptr;
}

bool SoccerVisualizer::ProcessEvents(bool &quit) {
  SDL_Event event;
  while (SDL_PollEvent(&event) != 0) {
    if (event.type == SDL_QUIT) {
      quit = true;
    }
  }

  return true;
}

void SoccerVisualizer::Cleanup() {
  SDL_DestroyRenderer(renderer_);
  SDL_DestroyWindow(window_);
  renderer_ = nullptr;
  window_ = nullptr;
  SDL_Quit();
}

void SoccerVisualizer::RenderFrame(const SoccerPitch &pitch) {
  Clear();
  DrawPitch();

  if (pitch.p_RedTeam != nullptr) {
    for (std::size_t i = 0; i < pitch.p_RedTeam->m_Players.size(); ++i) {
      if (pitch.p_RedTeam->m_Players[i] != nullptr) {
        DrawPlayer(*pitch.p_RedTeam->m_Players[i], true,
                   i == GOALKEEPER_INDEX);
      }
    }
  }

  if (pitch.p_BlueTeam != nullptr) {
    for (std::size_t i = 0; i < pitch.p_BlueTeam->m_Players.size(); ++i) {
      if (pitch.p_BlueTeam->m_Players[i] != nullptr) {
        DrawPlayer(*pitch.p_BlueTeam->m_Players[i], false,
                   i == GOALKEEPER_INDEX);
      }
    }
  }

  if (pitch.p_Ball != nullptr) {
    const Vector ball_pos = pitch.p_Ball->Pos();
    DrawBall(static_cast<float>(ball_pos.x), static_cast<float>(ball_pos.y));
  }

  if (pitch.p_RedGoal != nullptr && pitch.p_BlueGoal != nullptr) {
    const int redScore = pitch.p_BlueGoal->Score();
    const int blueScore = pitch.p_RedGoal->Score();
    DrawScoreboard(redScore, blueScore);
  }

  SDL_RenderPresent(renderer_);
}

float SoccerVisualizer::Length(float x, float y) {
  return std::sqrt((x * x) + (y * y));
}

void SoccerVisualizer::Clear() {
  SDL_SetRenderDrawColor(renderer_, 20, 28, 20, 255);
  SDL_RenderClear(renderer_);
}

void SoccerVisualizer::DrawThickLine(int x1, int y1, int x2, int y2,
                                     int thickness) {
  const int radius = thickness / 2;
  for (int ox = -radius; ox <= radius; ++ox) {
    for (int oy = -radius; oy <= radius; ++oy) {
      if (std::abs(ox) + std::abs(oy) <= radius + 1) {
        SDL_RenderDrawLine(renderer_, x1 + ox, y1 + oy, x2 + ox, y2 + oy);
      }
    }
  }
}

void SoccerVisualizer::DrawRectOutline(const SDL_Rect &rect, int thickness) {
  for (int i = 0; i < thickness; ++i) {
    SDL_Rect r{rect.x - i, rect.y - i, rect.w + (i * 2), rect.h + (i * 2)};
    SDL_RenderDrawRect(renderer_, &r);
  }
}

void SoccerVisualizer::DrawCircleOutline(int cx, int cy, int radius,
                                         int thickness) {
  for (int t = 0; t < thickness; ++t) {
    int x = 0;
    int y = radius - t;
    int d = 3 - (2 * y);

    while (y >= x) {
      SDL_RenderDrawPoint(renderer_, cx + x, cy + y);
      SDL_RenderDrawPoint(renderer_, cx - x, cy + y);
      SDL_RenderDrawPoint(renderer_, cx + x, cy - y);
      SDL_RenderDrawPoint(renderer_, cx - x, cy - y);
      SDL_RenderDrawPoint(renderer_, cx + y, cy + x);
      SDL_RenderDrawPoint(renderer_, cx - y, cy + x);
      SDL_RenderDrawPoint(renderer_, cx + y, cy - x);
      SDL_RenderDrawPoint(renderer_, cx - y, cy - x);

      ++x;
      if (d > 0) {
        --y;
        d += (4 * (x - y)) + 10;
      } else {
        d += (4 * x) + 6;
      }
    }
  }
}

void SoccerVisualizer::DrawFilledCircle(int cx, int cy, int radius) {
  for (int y = -radius; y <= radius; ++y) {
    const int dx =
        static_cast<int>(std::sqrt((radius * radius) - (y * y)));
    SDL_RenderDrawLine(renderer_, cx - dx, cy + y, cx + dx, cy + y);
  }
}

void SoccerVisualizer::FillTriangle(IntPoint p0, IntPoint p1, IntPoint p2) {
  std::array<IntPoint, 3> pts = {p0, p1, p2};
  std::sort(pts.begin(), pts.end(), [](const IntPoint &a, const IntPoint &b) {
    return a.y < b.y;
  });

  p0 = pts[0];
  p1 = pts[1];
  p2 = pts[2];

  const int totalHeight = p2.y - p0.y;
  if (totalHeight == 0) {
    return;
  }

  for (int y = p0.y; y <= p2.y; ++y) {
    const bool secondHalf = y > p1.y || p1.y == p0.y;
    const int segmentHeight = secondHalf ? (p2.y - p1.y) : (p1.y - p0.y);
    if (segmentHeight == 0) {
      continue;
    }

    const float alpha =
        static_cast<float>(y - p0.y) / static_cast<float>(totalHeight);
    const float beta = static_cast<float>(y - (secondHalf ? p1.y : p0.y)) /
                       static_cast<float>(segmentHeight);

    IntPoint A{
        p0.x + static_cast<int>((p2.x - p0.x) * alpha),
        y,
    };
    IntPoint B = secondHalf
                     ? IntPoint{p1.x + static_cast<int>((p2.x - p1.x) * beta),
                                y}
                     : IntPoint{p0.x + static_cast<int>((p1.x - p0.x) * beta),
                                y};

    if (A.x > B.x) {
      std::swap(A.x, B.x);
    }
    SDL_RenderDrawLine(renderer_, A.x, y, B.x, y);
  }
}

void SoccerVisualizer::DrawGoalNet(const SDL_Rect &rect) {
  SDL_SetRenderDrawColor(renderer_, 170, 170, 170, 255);
  for (int y = rect.y + GOAL_NET_START_OFFSET; y < rect.y + rect.h;
       y += GOAL_NET_SPACING) {
    SDL_RenderDrawLine(renderer_, rect.x + GOAL_NET_INSET, y,
                       rect.x + rect.w - GOAL_NET_INSET, y);
  }
  for (int x = rect.x + GOAL_NET_START_OFFSET; x < rect.x + rect.w;
       x += GOAL_NET_SPACING) {
    SDL_RenderDrawLine(renderer_, x, rect.y + GOAL_NET_INSET, x,
                       rect.y + rect.h - GOAL_NET_INSET);
  }

  SDL_SetRenderDrawColor(renderer_, 245, 245, 245, 255);
  DrawRectOutline(rect, LINE_THICKNESS);
}

void SoccerVisualizer::DrawPitch() {
  SDL_Rect pitch{PITCH_LEFT, PITCH_TOP, PITCH_WIDTH, PITCH_HEIGHT};

  SDL_SetRenderDrawColor(renderer_, 42, 128, 52, 255);
  SDL_RenderFillRect(renderer_, &pitch);

  const int stripeWidth = PITCH_WIDTH / PITCH_STRIPE_COUNT;
  for (int i = 0; i < PITCH_STRIPE_COUNT; ++i) {
    if (i % 2 == 0) {
      SDL_SetRenderDrawColor(renderer_, 48, 140, 58, 255);
    } else {
      SDL_SetRenderDrawColor(renderer_, 36, 118, 48, 255);
    }
    SDL_Rect stripe{PITCH_LEFT + (i * stripeWidth), PITCH_TOP, stripeWidth,
                    PITCH_HEIGHT};
    SDL_RenderFillRect(renderer_, &stripe);
  }

  SDL_SetRenderDrawColor(renderer_, 245, 245, 245, 255);
  DrawRectOutline(pitch, LINE_THICKNESS);
  DrawThickLine(FIELD_CENTER_X, PITCH_TOP, FIELD_CENTER_X, PITCH_BOTTOM,
                LINE_THICKNESS);

  DrawCircleOutline(FIELD_CENTER_X, FIELD_CENTER_Y, CENTER_CIRCLE_RADIUS,
                    LINE_THICKNESS);
  DrawFilledCircle(FIELD_CENTER_X, FIELD_CENTER_Y, CENTER_SPOT_RADIUS);

  const int penaltyTop = FIELD_CENTER_Y - (PENALTY_BOX_HEIGHT / 2);
  const int goalBoxTop = FIELD_CENTER_Y - (GOAL_BOX_HEIGHT / 2);

  SDL_Rect leftPenalty{PITCH_LEFT, penaltyTop, PENALTY_BOX_DEPTH,
                       PENALTY_BOX_HEIGHT};
  SDL_Rect rightPenalty{PITCH_RIGHT - PENALTY_BOX_DEPTH, penaltyTop,
                        PENALTY_BOX_DEPTH, PENALTY_BOX_HEIGHT};
  SDL_Rect leftGoalBox{PITCH_LEFT, goalBoxTop, GOAL_BOX_DEPTH, GOAL_BOX_HEIGHT};
  SDL_Rect rightGoalBox{PITCH_RIGHT - GOAL_BOX_DEPTH, goalBoxTop,
                        GOAL_BOX_DEPTH, GOAL_BOX_HEIGHT};

  SDL_RenderDrawRect(renderer_, &leftPenalty);
  SDL_RenderDrawRect(renderer_, &rightPenalty);
  SDL_RenderDrawRect(renderer_, &leftGoalBox);
  SDL_RenderDrawRect(renderer_, &rightGoalBox);

  DrawFilledCircle(PITCH_LEFT + PENALTY_MARK_OFFSET_X, FIELD_CENTER_Y,
                   LINE_THICKNESS);
  DrawFilledCircle(PITCH_RIGHT - PENALTY_MARK_OFFSET_X, FIELD_CENTER_Y,
                   LINE_THICKNESS);

  SDL_Rect leftGoalRect{PITCH_LEFT - GOAL_DEPTH, GOAL_TOP_Y, GOAL_DEPTH,
                        GOAL_HEIGHT};
  SDL_Rect rightGoalRect{PITCH_RIGHT, GOAL_TOP_Y, GOAL_DEPTH, GOAL_HEIGHT};
  DrawGoalNet(leftGoalRect);
  DrawGoalNet(rightGoalRect);
}

void SoccerVisualizer::DrawPlayer(const Player &player, bool isRedTeam,
                                  bool isGoalKeeper) {
  const Vector heading = player.Heading();
  float headingX = static_cast<float>(heading.x);
  float headingY = static_cast<float>(heading.y);

  const float len = Length(headingX, headingY);
  if (len < 0.001f) {
    headingX = isRedTeam ? 1.0f : -1.0f;
    headingY = 0.0f;
  } else {
    headingX /= len;
    headingY /= len;
  }

  const float angle = std::atan2(headingY, headingX);
  const float size = isGoalKeeper ? PLAYER_SIZE_GOALKEEPER : PLAYER_SIZE_FIELD;

  const Vector pos = player.Pos();
  IntPoint p1{static_cast<int>(std::lround(pos.x + std::cos(angle) * size)),
              static_cast<int>(std::lround(pos.y + std::sin(angle) * size))};
  IntPoint p2{static_cast<int>(std::lround(
                  pos.x + std::cos(angle + PLAYER_WING_ANGLE_OFFSET) * size)),
              static_cast<int>(std::lround(
                  pos.y + std::sin(angle + PLAYER_WING_ANGLE_OFFSET) * size))};
  IntPoint p3{static_cast<int>(std::lround(
                  pos.x + std::cos(angle - PLAYER_WING_ANGLE_OFFSET) * size)),
              static_cast<int>(std::lround(
                  pos.y + std::sin(angle - PLAYER_WING_ANGLE_OFFSET) * size))};

  if (isRedTeam) {
    SDL_SetRenderDrawColor(renderer_, isGoalKeeper ? 236 : 220,
                           isGoalKeeper ? 121 : 56, isGoalKeeper ? 34 : 48,
                           255);
  } else {
    SDL_SetRenderDrawColor(renderer_, isGoalKeeper ? 76 : 52,
                           isGoalKeeper ? 148 : 114, isGoalKeeper ? 230 : 216,
                           255);
  }
  FillTriangle(p1, p2, p3);

  SDL_SetRenderDrawColor(renderer_, 12, 12, 12, 255);
  DrawThickLine(p1.x, p1.y, p2.x, p2.y, PLAYER_OUTLINE_THICKNESS);
  DrawThickLine(p2.x, p2.y, p3.x, p3.y, PLAYER_OUTLINE_THICKNESS);
  DrawThickLine(p3.x, p3.y, p1.x, p1.y, PLAYER_OUTLINE_THICKNESS);
}

void SoccerVisualizer::DrawBall(float x, float y) {
  const int xi = static_cast<int>(std::lround(x));
  const int yi = static_cast<int>(std::lround(y));

  SDL_SetRenderDrawColor(renderer_, 30, 30, 30, 255);
  DrawFilledCircle(xi + BALL_SHADOW_OFFSET_X, yi + BALL_SHADOW_OFFSET_Y,
                   BALL_RADIUS);

  SDL_SetRenderDrawColor(renderer_, 250, 250, 250, 255);
  DrawFilledCircle(xi, yi, BALL_RADIUS);

  SDL_SetRenderDrawColor(renderer_, 10, 10, 10, 255);
  DrawCircleOutline(xi, yi, BALL_RADIUS, BALL_OUTLINE_THICKNESS);
}

void SoccerVisualizer::DrawDigit(int digit, int x, int y, int scale,
                                 SDL_Color color) {
  static constexpr int kSegmentMask[10] = {
      0b1111110, // 0
      0b0110000, // 1
      0b1101101, // 2
      0b1111001, // 3
      0b0110011, // 4
      0b1011011, // 5
      0b1011111, // 6
      0b1110000, // 7
      0b1111111, // 8
      0b1111011  // 9
  };

  if (digit < 0 || digit > 9) {
    return;
  }

  SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);

  const int w = 4 * scale;
  const int h = 7 * scale;
  const int t = std::max(1, scale / 2);
  const int mask = kSegmentMask[digit];

  auto drawH = [&](int sx, int sy, int sw) {
    SDL_Rect r{sx, sy, sw, t};
    SDL_RenderFillRect(renderer_, &r);
  };

  auto drawV = [&](int sx, int sy, int sh) {
    SDL_Rect r{sx, sy, t, sh};
    SDL_RenderFillRect(renderer_, &r);
  };

  if (mask & 0b1000000)
    drawH(x, y, w); // top
  if (mask & 0b0100000)
    drawV(x + w - t, y, h / 2); // top-right
  if (mask & 0b0010000)
    drawV(x + w - t, y + h / 2, h / 2); // bottom-right
  if (mask & 0b0001000)
    drawH(x, y + h - t, w); // bottom
  if (mask & 0b0000100)
    drawV(x, y + h / 2, h / 2); // bottom-left
  if (mask & 0b0000010)
    drawV(x, y, h / 2); // top-left
  if (mask & 0b0000001)
    drawH(x, y + (h / 2) - (t / 2), w); // middle
}

void SoccerVisualizer::DrawScoreboard(int redScore, int blueScore) {
  const int panelW = 160;
  const int panelH = 34;
  const int panelX = FIELD_CENTER_X - (panelW / 2);
  const int panelY = 10;

  SDL_SetRenderDrawColor(renderer_, 12, 16, 12, 220);
  SDL_Rect panel{panelX, panelY, panelW, panelH};
  SDL_RenderFillRect(renderer_, &panel);

  SDL_SetRenderDrawColor(renderer_, 235, 235, 235, 255);
  SDL_RenderDrawRect(renderer_, &panel);

  const int safeRed = std::clamp(redScore, 0, 99);
  const int safeBlue = std::clamp(blueScore, 0, 99);

  const SDL_Color redColor{228, 68, 58, 255};
  const SDL_Color blueColor{76, 132, 230, 255};
  const SDL_Color neutral{240, 240, 240, 255};

  const int scale = 3;
  const int digitY = panelY + 7;
  const int redX = panelX + 20;
  const int blueX = panelX + panelW - 48;

  DrawDigit(safeRed / 10, redX, digitY, scale, redColor);
  DrawDigit(safeRed % 10, redX + 18, digitY, scale, redColor);
  DrawDigit(safeBlue / 10, blueX, digitY, scale, blueColor);
  DrawDigit(safeBlue % 10, blueX + 18, digitY, scale, blueColor);

  SDL_SetRenderDrawColor(renderer_, neutral.r, neutral.g, neutral.b, neutral.a);
  SDL_Rect dotTop{FIELD_CENTER_X - 2, panelY + 12, 4, 4};
  SDL_Rect dotBottom{FIELD_CENTER_X - 2, panelY + 20, 4, 4};
  SDL_RenderFillRect(renderer_, &dotTop);
  SDL_RenderFillRect(renderer_, &dotBottom);
}
