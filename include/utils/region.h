#pragma once
#include "../math/vector.h"
#include "utils.h"

class Region {
private:
    double m_Top;
    double m_Bottom;
    double m_Left;
    double m_Right;

    double m_Width;
    double m_Height;

    Vector m_Center;

    int m_Id;
public:

    Region(): m_Top(0), m_Bottom(0), m_Left(0), m_Right(0),
        m_Width(0), m_Height(0), m_Center(0, 0), m_Id(0) {}

    Region(double top, double bottom, double left, double right, int id)
        : m_Top(top), m_Bottom(bottom), m_Left(left), m_Right(right),
        m_Width(right - left), m_Height(top - bottom),
        m_Center((left + right) * 0.5, (top + bottom) * 0.5), m_Id(id) {}

    inline Vector GetRandomPos() const;

    double Top() const { return m_Top; }
    double Bottom() const { return m_Bottom; }
    double Left() const { return m_Left; }
    double Right() const { return m_Right; }

    double Width() const { return m_Width; }
    double Height() const { return m_Height; }
    Vector Center() const { return m_Center; }
    int Id() const { return m_Id; }

    inline bool Inside(const Vector& pos) const;
};


bool Region::Inside(const Vector& pos) const {
    // if out of bounds.
    if(pos.x > m_Right || pos.x < m_Left || pos.y > m_Bottom || pos.y < m_Top)
        return false;

    return true;
}

Vector Region::GetRandomPos() const {
    return Vector(Utils::Range(m_Left, m_Right), Utils::Range(m_Bottom, m_Top));
}
