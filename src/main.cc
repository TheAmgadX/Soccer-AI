#include <SDL.h>

#include <algorithm>
#include <array>
#include <cmath>

#include "utils/constants.h"

using namespace constants;

struct Entity {
    float x;
    float y;
    float dirX;
    float dirY;
};

struct IntPoint {
    int x;
    int y;
};

class SoccerVisualizer {
public:
    bool Init() {
        if (SDL_Init(SDL_INIT_VIDEO) < 0) {
            return false;
        }

        window_ = SDL_CreateWindow("Pixel Soccer AI", SDL_WINDOWPOS_CENTERED,
                                   SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH,
                                   SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

        ResetDemoState();
        return window_ != nullptr && renderer_ != nullptr;
    }

    void Cleanup() {
        SDL_DestroyRenderer(renderer_);
        SDL_DestroyWindow(window_);
        SDL_Quit();
    }

    void UpdateRedPlayer(int idx, float x, float y, float dirX, float dirY) {
        if (idx < 0 || idx >= PLAYERS_PER_TEAM) {
            return;
        }
        redTeam_[idx] = {x, y, dirX, dirY};
    }

    void UpdateBluePlayer(int idx, float x, float y, float dirX, float dirY) {
        if (idx < 0 || idx >= PLAYERS_PER_TEAM) {
            return;
        }
        blueTeam_[idx] = {x, y, dirX, dirY};
    }

    void UpdateBall(float x, float y) {
        ball_.x = x;
        ball_.y = y;
    }

    void RenderFrame() {
        Clear();
        DrawPitch();

        for (int i = 0; i < PLAYERS_PER_TEAM; ++i) {
            DrawPlayer(redTeam_[i], true, i == GOALKEEPER_INDEX);
            DrawPlayer(blueTeam_[i], false, i == GOALKEEPER_INDEX);
        }

        DrawBall(ball_);
        SDL_RenderPresent(renderer_);
    }

    void ResetDemoState() {
        ball_ = {static_cast<float>(BALL_START_X), static_cast<float>(BALL_START_Y), 0.0f, 0.0f};

        for (int i = 0; i < PLAYERS_PER_TEAM; ++i) {
            redTeam_[i] = {static_cast<float>(RED_TEAM_START_X + i * TEAM_X_SPACING),
                           static_cast<float>(TEAM_START_Y + i * TEAM_Y_SPACING), 1.0f, 0.0f};
            blueTeam_[i] = {static_cast<float>(BLUE_TEAM_START_X - i * TEAM_X_SPACING),
                            static_cast<float>(TEAM_START_Y + i * TEAM_Y_SPACING), -1.0f, 0.0f};
        }
    }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;

    std::array<Entity, PLAYERS_PER_TEAM> redTeam_{};
    std::array<Entity, PLAYERS_PER_TEAM> blueTeam_{};
    Entity ball_{};

    static float Length(float x, float y) {
        return std::sqrt((x * x) + (y * y));
    }

    void Clear() {
        SDL_SetRenderDrawColor(renderer_, 20, 28, 20, 255);
        SDL_RenderClear(renderer_);
    }

    void DrawThickLine(int x1, int y1, int x2, int y2, int thickness) {
        const int radius = thickness / 2;
        for (int ox = -radius; ox <= radius; ++ox) {
            for (int oy = -radius; oy <= radius; ++oy) {
                if (std::abs(ox) + std::abs(oy) <= radius + 1) {
                    SDL_RenderDrawLine(renderer_, x1 + ox, y1 + oy, x2 + ox,
                                       y2 + oy);
                }
            }
        }
    }

    void DrawRectOutline(const SDL_Rect& rect, int thickness) {
        for (int i = 0; i < thickness; ++i) {
            SDL_Rect r{rect.x - i, rect.y - i, rect.w + (i * 2),
                       rect.h + (i * 2)};
            SDL_RenderDrawRect(renderer_, &r);
        }
    }

    void DrawCircleOutline(int cx, int cy, int radius, int thickness) {
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

    void DrawFilledCircle(int cx, int cy, int radius) {
        for (int y = -radius; y <= radius; ++y) {
            const int dx = static_cast<int>(std::sqrt((radius * radius) - (y * y)));
            SDL_RenderDrawLine(renderer_, cx - dx, cy + y, cx + dx, cy + y);
        }
    }

    void FillTriangle(IntPoint p0, IntPoint p1, IntPoint p2) {
        std::array<IntPoint, 3> pts = {p0, p1, p2};
        std::sort(pts.begin(), pts.end(), [](const IntPoint& a, const IntPoint& b) {
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

            const float alpha = static_cast<float>(y - p0.y) / static_cast<float>(totalHeight);
            const float beta = static_cast<float>(y - (secondHalf ? p1.y : p0.y)) /
                               static_cast<float>(segmentHeight);

            IntPoint A{
                p0.x + static_cast<int>((p2.x - p0.x) * alpha),
                y,
            };
            IntPoint B = secondHalf
                             ? IntPoint{p1.x + static_cast<int>((p2.x - p1.x) * beta), y}
                             : IntPoint{p0.x + static_cast<int>((p1.x - p0.x) * beta), y};

            if (A.x > B.x) {
                std::swap(A.x, B.x);
            }
            SDL_RenderDrawLine(renderer_, A.x, y, B.x, y);
        }
    }

    void DrawGoalNet(const SDL_Rect& rect) {
        SDL_SetRenderDrawColor(renderer_, 170, 170, 170, 255);
        for (int y = rect.y + GOAL_NET_START_OFFSET; y < rect.y + rect.h; y += GOAL_NET_SPACING) {
            SDL_RenderDrawLine(renderer_, rect.x + GOAL_NET_INSET, y, rect.x + rect.w - GOAL_NET_INSET, y);
        }
        for (int x = rect.x + GOAL_NET_START_OFFSET; x < rect.x + rect.w; x += GOAL_NET_SPACING) {
            SDL_RenderDrawLine(renderer_, x, rect.y + GOAL_NET_INSET, x, rect.y + rect.h - GOAL_NET_INSET);
        }

        SDL_SetRenderDrawColor(renderer_, 245, 245, 245, 255);
        DrawRectOutline(rect, LINE_THICKNESS);
    }

    void DrawPitch() {
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
        DrawThickLine(FIELD_CENTER_X, PITCH_TOP, FIELD_CENTER_X, PITCH_BOTTOM, LINE_THICKNESS);

        DrawCircleOutline(FIELD_CENTER_X, FIELD_CENTER_Y, CENTER_CIRCLE_RADIUS, LINE_THICKNESS);
        DrawFilledCircle(FIELD_CENTER_X, FIELD_CENTER_Y, CENTER_SPOT_RADIUS);

        const int penaltyTop = FIELD_CENTER_Y - (PENALTY_BOX_HEIGHT / 2);
        const int goalBoxTop = FIELD_CENTER_Y - (GOAL_BOX_HEIGHT / 2);

        SDL_Rect leftPenalty{PITCH_LEFT, penaltyTop, PENALTY_BOX_DEPTH,
                             PENALTY_BOX_HEIGHT};
        SDL_Rect rightPenalty{PITCH_RIGHT - PENALTY_BOX_DEPTH, penaltyTop,
                              PENALTY_BOX_DEPTH, PENALTY_BOX_HEIGHT};
        SDL_Rect leftGoalBox{PITCH_LEFT, goalBoxTop, GOAL_BOX_DEPTH,
                             GOAL_BOX_HEIGHT};
        SDL_Rect rightGoalBox{PITCH_RIGHT - GOAL_BOX_DEPTH, goalBoxTop,
                              GOAL_BOX_DEPTH, GOAL_BOX_HEIGHT};

        SDL_RenderDrawRect(renderer_, &leftPenalty);
        SDL_RenderDrawRect(renderer_, &rightPenalty);
        SDL_RenderDrawRect(renderer_, &leftGoalBox);
        SDL_RenderDrawRect(renderer_, &rightGoalBox);

        DrawFilledCircle(PITCH_LEFT + PENALTY_MARK_OFFSET_X, FIELD_CENTER_Y, LINE_THICKNESS);
        DrawFilledCircle(PITCH_RIGHT - PENALTY_MARK_OFFSET_X, FIELD_CENTER_Y, LINE_THICKNESS);

        SDL_Rect leftGoalRect{PITCH_LEFT - GOAL_DEPTH, GOAL_TOP_Y, GOAL_DEPTH,
                              GOAL_HEIGHT};
        SDL_Rect rightGoalRect{PITCH_RIGHT, GOAL_TOP_Y, GOAL_DEPTH, GOAL_HEIGHT};
        DrawGoalNet(leftGoalRect);
        DrawGoalNet(rightGoalRect);
    }

    void DrawPlayer(const Entity& player, bool isRedTeam, bool isGoalKeeper) {
        float headingX = player.dirX;
        float headingY = player.dirY;
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

        IntPoint p1{static_cast<int>(std::lround(player.x + std::cos(angle) * size)),
                    static_cast<int>(std::lround(player.y + std::sin(angle) * size))};
        IntPoint p2{static_cast<int>(
                        std::lround(player.x + std::cos(angle + PLAYER_WING_ANGLE_OFFSET) * size)),
                    static_cast<int>(
                        std::lround(player.y + std::sin(angle + PLAYER_WING_ANGLE_OFFSET) * size))};
        IntPoint p3{static_cast<int>(
                        std::lround(player.x + std::cos(angle - PLAYER_WING_ANGLE_OFFSET) * size)),
                    static_cast<int>(
                        std::lround(player.y + std::sin(angle - PLAYER_WING_ANGLE_OFFSET) * size))};

        if (isRedTeam) {
            SDL_SetRenderDrawColor(renderer_, isGoalKeeper ? 236 : 220,
                                   isGoalKeeper ? 121 : 56,
                                   isGoalKeeper ? 34 : 48, 255);
        } else {
            SDL_SetRenderDrawColor(renderer_, isGoalKeeper ? 76 : 52,
                                   isGoalKeeper ? 148 : 114,
                                   isGoalKeeper ? 230 : 216, 255);
        }
        FillTriangle(p1, p2, p3);

        SDL_SetRenderDrawColor(renderer_, 12, 12, 12, 255);
        DrawThickLine(p1.x, p1.y, p2.x, p2.y, PLAYER_OUTLINE_THICKNESS);
        DrawThickLine(p2.x, p2.y, p3.x, p3.y, PLAYER_OUTLINE_THICKNESS);
        DrawThickLine(p3.x, p3.y, p1.x, p1.y, PLAYER_OUTLINE_THICKNESS);
    }

    void DrawBall(const Entity& ball) {
        const int x = static_cast<int>(std::lround(ball.x));
        const int y = static_cast<int>(std::lround(ball.y));

        SDL_SetRenderDrawColor(renderer_, 30, 30, 30, 255);
        DrawFilledCircle(x + BALL_SHADOW_OFFSET_X, y + BALL_SHADOW_OFFSET_Y, BALL_RADIUS);

        SDL_SetRenderDrawColor(renderer_, 250, 250, 250, 255);
        DrawFilledCircle(x, y, BALL_RADIUS);

        SDL_SetRenderDrawColor(renderer_, 10, 10, 10, 255);
        DrawCircleOutline(x, y, BALL_RADIUS, BALL_OUTLINE_THICKNESS);
    }
};

int main(int argc, char* args[]) {
    (void)argc;
    (void)args;

    SoccerVisualizer graphics;
    
    if (!graphics.Init()) {
        return -1;
    }

    bool quit = false;
    SDL_Event event;

    while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Use these update methods from your AI/game loop each frame.
        // Example:
        // graphics.UpdateRedPlayer(i, x, y, dirX, dirY);
        // graphics.UpdateBluePlayer(i, x, y, dirX, dirY);
        // graphics.UpdateBall(ballX, ballY);

        graphics.RenderFrame();
        SDL_Delay(FRAME_DELAY_MS);
    }

    graphics.Cleanup();
    return 0;
}
