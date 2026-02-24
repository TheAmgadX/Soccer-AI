#pragma once
#include "../math/vector.h"

class Goal {
private:
    Vector m_Right;
    Vector m_Left;
    Vector m_Facing;
    Vector m_Center;

    int m_Score;
public:
    Goal(Vector right, Vector left, Vector facing) :
        m_Right(right), m_Left(left), m_Facing(facing), m_Center((right + left) * 0.5), m_Score(0) {}

    ~Goal() {}

    inline bool CheckGoal(const Vector ballPosition) {
        if(ballPosition.x == m_Right.x){
            // since the left and right changes based on the facing, we must consider both cases.

            // if the left goal.
            if(ballPosition.y >= m_Right.y && ballPosition.y <= m_Left.y){
                m_Score++;
                return true;
            }

            // if the right goal.
            if(ballPosition.y >= m_Left.y && ballPosition.y <= m_Right.y){
                m_Score++;
                return true;
            }
        }

        return false;
    }

    inline Vector Right() const {
        return m_Right;
    }

    inline Vector Left() const {
        return m_Left;
    }

    inline Vector Center() const {
        return m_Center;
    }

    inline Vector Facing() const {
        return m_Facing;
    }

    inline int Score() const {
        return m_Score;
    }
};
