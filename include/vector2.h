#pragma once

#include <cmath>
#undef abs

class Vect2 {
public:
    Vect2() : _x(0.), _y(0.) {}
    Vect2(float x, float y) : _x(x), _y(y) {}

    float norm1() const { return std::abs(_x) + std::abs(_y); }
    float norm2() const { return std::sqrt(_x*_x+_y*_y); }
    void normalize() { _x/=norm2(); _y/=norm2(); }
    Vect2 normalized() const { Vect2 out=*this; out.normalize(); return out; }

    Vect2 operator-(const Vect2& other) const { return {_x-other._x, _y-other._y}; }
    Vect2 operator+(const Vect2& other) const { return {_x+other._x, _y+other._y}; }
    Vect2 operator*(float f) const { return {_x*f, _y*f}; }

    bool operator==(const Vect2& other) const { return std::abs(_x-other._x) < 1e-5 && std::abs(_y-other._y)<1e-5; }
    bool operator!=(const Vect2& other) const { return !(*this == other); }

    float x() const { return _x;}
    float y() const { return _y;}
    void set_x(float x) { _x = x;}
    void set_y(float y) { _y = y;}

    float dot(const Vect2& other) const { return _x*other._x + _y*other._y; }
    float cross(const Vect2& other) const { return _x*other._y - _y*other._x; }
    float angle(const Vect2& m) const { return std::atan2(cross(m), dot(m)); }
    float angle() const { return std::atan2(_y, _x); }
private:
    float _x, _y;
};