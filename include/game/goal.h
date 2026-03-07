#pragma once
#include <algorithm>
#include "../math/vector.h"
#include "../utils/constants.h"

class Goal {
private:
    Vector m_Right;
    Vector m_Left;
    Vector m_Facing;
    Vector m_Center;

    int m_Score;

public:
    Goal(Vector right, Vector left, Vector facing)
        : m_Right(right), m_Left(left), m_Facing(facing),
            m_Center(Vector(right.x, constants::FIELD_CENTER_Y)), m_Score(0) {}

    ~Goal() {}

    inline bool CheckGoal(const Vector ballPosition) {
        const double top_y = std::min(m_Right.y, m_Left.y);
        const double bottom_y = std::max(m_Right.y, m_Left.y);
        const bool inside_goal_y = ballPosition.y >= top_y && ballPosition.y <= bottom_y;

        if (!inside_goal_y) {
            return false;
        }

        // Left goal faces right (+x), right goal faces left (-x).
        if (m_Facing.x > 0.0) {
            if (ballPosition.x <= m_Right.x + constants::BALL_RADIUS) {
                m_Score++;
                return true;
            }
        } else {
            if (ballPosition.x >= m_Right.x - constants::BALL_RADIUS) {
                m_Score++;
                return true;
            }
        }

        return false;
    }

    inline Vector Right() const { return m_Right; }

    inline Vector Left() const { return m_Left; }

    inline Vector Center() const { return m_Center; }

    inline Vector Facing() const { return m_Facing; }

    inline int Score() const { return m_Score; }
};
