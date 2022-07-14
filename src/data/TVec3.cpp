//
// Created by xhy on 2022/5/17.
//

#include "TVec3.h"

#include <cmath>

#include "TBlockPos.h"

namespace trapdoor {
    TVec3::TVec3(int _x) : TVec3(_x, _x, _x) {}

    TVec3::TVec3(float _x) : TVec3(_x, _x, _x) {}

    double TVec3::distanceTo(const TVec3 &vec3) const {
        return static_cast<double>(std::sqrt((vec3.x - x) * (vec3.x - x) +
                                             (vec3.y - y) * (vec3.y - y) +
                                             (vec3.z - z) * (vec3.z - z)));
    }

    std::string TVec3::toString() const {
        char buffer[64];
        sprintf(buffer, "[%.2f %.2f %.2f]", x, y, z);
        return std::string(buffer);
    }

    bool TVec3::operator!=(const TVec3 &v) const { return x != v.x || y != v.y || z != v.z; }

    bool TVec3::operator==(const TVec3 &v) const { return x == v.x && y == v.y && z == v.z; }

    TBlockPos TVec3::toBlockPos() const {
        auto cx = x < 0 ? x - 1 : x;
        auto cy = y < 0 ? y - 1 : y;
        auto cz = z < 0 ? z - 1 : z;
        return {cx, cy, cz};
    }

    bool TVec3::operator<(const TVec3 &rhs) const {
        if (x < rhs.x) return true;
        if (rhs.x < x) return false;
        if (y < rhs.y) return true;
        if (rhs.y < y) return false;
        return z < rhs.z;
    }

    TVec3 TVec3::operator+(const TVec3 &v) const {
        return {this->x + v.x, this->y + v.y, this->z + v.z};
    }

    TVec3 TVec3::operator-(const TVec3 &v) const {
        return {this->x - v.x, this->y - v.y, this->z - v.z};
    }

    TVec3 TVec3::operator*(float times) const { return {x * times, y * times, z * times}; }

}  // namespace trapdoor