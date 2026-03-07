#pragma once

#include <SDL.h>

class Player;
class SoccerPitch;

class SoccerVisualizer {
public:
  SoccerVisualizer() = default;
  ~SoccerVisualizer() = default;

  bool Init();
  bool ProcessEvents(bool &quit);
  void RenderFrame(const SoccerPitch &pitch);
  void Cleanup();

private:
  struct IntPoint {
    int x;
    int y;
  };

  SDL_Window *window_ = nullptr;
  SDL_Renderer *renderer_ = nullptr;

  static float Length(float x, float y);

  void Clear();
  void DrawPitch();
  void DrawPlayer(const Player &player, bool isRedTeam, bool isGoalKeeper);
  void DrawBall(float x, float y);
  void DrawDigit(int digit, int x, int y, int scale, SDL_Color color);
  void DrawScoreboard(int redScore, int blueScore);

  void DrawThickLine(int x1, int y1, int x2, int y2, int thickness);
  void DrawRectOutline(const SDL_Rect &rect, int thickness);
  void DrawCircleOutline(int cx, int cy, int radius, int thickness);
  void DrawFilledCircle(int cx, int cy, int radius);
  void FillTriangle(IntPoint p0, IntPoint p1, IntPoint p2);
  void DrawGoalNet(const SDL_Rect &rect);
};
