#pragma once
#include "../math/vector.h"
#include "constants.h"
#include <vector>

class Wall {
    Vector m_Start, m_End, m_Normal;

    void CalculateNormal() {
        Vector tmp = VecNormalize(m_End - m_Start);

        m_Normal.x = -tmp.y;
        m_Normal.y = tmp.x;
    }

public:
    Wall(Vector _start, Vector _end) : m_Start(_start), m_End(_end) {
        CalculateNormal();
    }

    static std::vector<Wall>& CreateWalls() {
        static std::vector<Wall> walls;

        // if walls vector already created.
        if(!walls.empty())
            return walls;

        walls.reserve(4);

        double left_corner_up_x = constants::PITCH_MARGIN;
        double left_corner_up_y = constants::PITCH_MARGIN;

        double right_corner_up_x = constants::PITCH_WIDTH + constants::PITCH_MARGIN;
        double right_corner_up_y = constants::PITCH_MARGIN;

        double left_corner_down_x = constants::PITCH_MARGIN;
        double left_corner_down_y = constants::PITCH_HEIGHT + constants::PITCH_MARGIN;

        double right_corner_down_x = constants::PITCH_WIDTH + constants::PITCH_MARGIN;
        double right_corner_down_y = constants::PITCH_HEIGHT + constants::PITCH_MARGIN;

        // bottom (ordered right->left so normal points up, into the pitch)
        walls.emplace_back(Wall(Vector(right_corner_down_x, right_corner_down_y),
            Vector(left_corner_down_x, left_corner_down_y)));

        // right
        walls.emplace_back(Wall(Vector(right_corner_up_x, right_corner_up_y),
            Vector(right_corner_down_x, right_corner_down_y)));

        // top
        walls.emplace_back(Wall(Vector(left_corner_up_x, left_corner_up_y),
            Vector(right_corner_up_x, right_corner_up_y)));

        // left (ordered down->up so normal points right, into the pitch)
        walls.emplace_back(Wall(Vector(left_corner_down_x, left_corner_down_y),
            Vector(left_corner_up_x, left_corner_up_y)));

        return walls;
    }

    Vector From() const {
        return m_Start;
    }

    Vector To() const {
        return m_End;
    }

    Vector Normal() const {
        return m_Normal;
    }
};
