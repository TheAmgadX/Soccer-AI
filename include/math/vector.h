#pragma once

#include "utils/constants.h"
#include <cmath>

class Vector {
public:
private:
    enum { clock_wise = 1, anti_clock_wise = -1 };

    inline bool isEqual(double a, double b) const {
        if (fabs(a - b) < constants::Epsilon) {
            return true;
        }

        return false;
    }

public:
    double x, y;

    Vector() : x(0.0), y(0.0) {}

    Vector(double _x, double _y) : x(_x), y(_y) {}

    inline void Zero() { x = 0.0, y = 0.0; }

    inline bool isZero() { return (x * x + y * y) < constants::MinDouble; }

    inline double Length() const;

    inline double LengthSQ() const;

    inline void Normalize();

    inline double Dot(const Vector &vec) const;

    inline int Sign(const Vector &vec) const;

    inline Vector Perp() const;

    inline Vector GetReverse() const;

    inline void Truncate(double max);

    inline double Distance(const Vector &vec) const;

    inline double DistanceSQ(const Vector &vec) const;

    inline void Reflect(const Vector &norm);

    const Vector &operator+=(const Vector &rhs) {
        x += rhs.x;
        y += rhs.y;

        return *this;
    }

    const Vector &operator-=(const Vector &rhs) {
        x -= rhs.x;
        y -= rhs.y;

        return *this;
    }

    const Vector &operator*=(const double &rhs) {
        x *= rhs;
        y *= rhs;

        return *this;
    }

    const Vector &operator/=(const double &rhs) {
        x /= rhs;
        y /= rhs;

        return *this;
    }

    bool operator==(const Vector &rhs) const {
        return isEqual(x, rhs.x) && isEqual(y, rhs.y);
    }

    bool operator!=(const Vector &rhs) const {
        return (x != rhs.x) || (y != rhs.y);
    }

}; // Vector class.

//
//  --------------- Helper Operators and Functions ---------------
//
inline Vector operator+(const Vector &lhs, const Vector &rhs) {
    Vector vec = lhs;
    vec += rhs;

    return vec;
}

inline Vector operator-(const Vector &lhs, const Vector &rhs) {
    Vector vec = lhs;
    vec -= rhs;

    return vec;
}

inline Vector operator*(double rhs, const Vector &lhs) {
    Vector vec = lhs;
    vec *= rhs;

    return vec;
}

inline Vector operator*(const Vector &rhs, double lhs) {
    Vector vec = rhs;
    vec *= lhs;

    return vec;
}

inline Vector operator/(double rhs, const Vector &lhs) {
    Vector vec = lhs;
    vec /= rhs;

    return vec;
}

inline Vector operator/(const Vector &rhs, double lhs) {
    Vector vec = rhs;
    vec /= lhs;

    return vec;
}

inline Vector VecNormalize(const Vector &v) {
    Vector vec = v;
    double len = vec.Length();

    if (len > constants::Epsilon) {
        vec.x /= len;
        vec.y /= len;
    }

    return vec;
}

inline double VecDistanceSQ(const Vector &v1, const Vector &v2) {
    double ySeparation = v2.y - v1.y;
    double xSeparation = v2.x - v1.x;

    return ySeparation * ySeparation + xSeparation * xSeparation;
}

inline double Vec2DDistance(const Vector &v1, const Vector &v2) {
    return sqrt(VecDistanceSQ(v1, v2));
}

inline double VecLengthSQ(const Vector &v) { return v.x * v.x + v.y * v.y; }

inline double VecLength(const Vector &v) { return sqrt(VecLengthSQ(v)); }

inline void WrapAround(Vector &pos, int MaxX, int MaxY) {
    if (pos.x > MaxX) {
        pos.x = 0.0;
    }

    if (pos.x < 0) {
        pos.x = (double)MaxX;
    }

    if (pos.y < 0) {
        pos.y = (double)MaxY;
    }

    if (pos.y > MaxY) {
        pos.y = 0.0;
    }
}

inline bool NotInsideRegion(Vector p, Vector top_left, Vector bot_rgt) {
    return (p.x < top_left.x) || (p.x > bot_rgt.x) || (p.y < top_left.y) ||
           (p.y > bot_rgt.y);
}

inline bool InsideRegion(Vector p, Vector top_left, Vector bot_rgt) {
    return !((p.x < top_left.x) || (p.x > bot_rgt.x) || (p.y < top_left.y) ||
             (p.y > bot_rgt.y));
}

/// @brief Transforms a point from World Space to the Agent's Local Space
inline Vector PointToLocalSpace(const Vector &point, const Vector &agentHeading,
                                const Vector &agentSide,
                                const Vector &agentPos) {
    Vector trans = point - agentPos;

    double localX = trans.Dot(agentHeading);
    double localY = trans.Dot(agentSide);

    return Vector(localX, localY);
}

/// @brief Transforms a point from Agent's Local Space back to World Space
inline Vector PointToWorldSpace(const Vector &localPoint,
                                const Vector &agentHeading,
                                const Vector &agentSide,
                                const Vector &agentPos) {
    return agentPos + (agentHeading * localPoint.x) + (agentSide * localPoint.y);
}

//
//  --------------- Class Methods ---------------
//
inline double Vector::LengthSQ() const { return (x * x + y * y); }

inline double Vector::Length() const { return std::sqrt(this->LengthSQ()); }

inline void Vector::Normalize() {
    double len = Length();

    if (len > constants::Epsilon) {
        x /= len;
        y /= len;
    }
}

inline double Vector::Dot(const Vector &vec) const {
    return x * vec.x + y * vec.y;
}

inline int Vector::Sign(const Vector &vec) const {
    if (y * vec.x > x * vec.y) {
        return anti_clock_wise;
    }

    return clock_wise;
}

inline Vector Vector::Perp() const { return Vector(-y, x); }

inline Vector Vector::GetReverse() const { return Vector(-x, -y); }

inline void Vector::Reflect(const Vector &norm) {
    *this += 2.0 * this->Dot(norm) * norm.GetReverse();
}

inline double Vector::Distance(const Vector &vec) const {
    return std::sqrt(this->DistanceSQ(vec));
}

inline double Vector::DistanceSQ(const Vector &vec) const {
    double x_diff = vec.x - x;
    double y_diff = vec.y - y;

    return x_diff * x_diff + y_diff * y_diff;
}

inline void Vector::Truncate(double max) {
    if (this->Length() > max) {
        this->Normalize();

        *this *= max;
    }
}
