//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_VEC3_H
#define TRAPDOOR_VEC3_H

#include <string>

namespace trapdoor {
    struct TBlockPos;

    struct TVec3 {
        float x = 0;
        float y = 0;
        float z = 0;

        TVec3() = default;

        explicit TVec3(int _x);

        explicit TVec3(float _x);

        TVec3(int _x, int _y, int _z) : x((float)_x), y((float)_y), z((float)_z) {}

        TVec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        bool operator==(const TVec3 &v) const;

        bool operator!=(const TVec3 &v) const;

        bool operator<(const TVec3 &v) const;

        TVec3 operator+(const TVec3 &v) const;

        TVec3 operator-(const TVec3 &v) const;

        TVec3 operator*(float times) const;

        double distanceTo(const TVec3 &vec3) const;

        std::string toString() const;

        TBlockPos toBlockPos() const;
    };
}  // namespace trapdoor

#endif  // TRAPDOOR_VEC3_H
