//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_VEC3_H
#define TRAPDOOR_VEC3_H

#include <string>

namespace tr {
    struct TBlockPos;

    struct TVec3 {
        float x;
        float y;
        float z;

        TVec3() = default;

        explicit TVec3(int _x);

        explicit TVec3(float _x);

        TVec3(int _x, int _y, int _z)
            : x((float)_x), y((float)_y), z((float)_z) {}

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

    struct TAABB {
        TVec3 p1;
        TVec3 p2;

        AABB(const TVec3 &p1_, const TVec3 &p2_) : p1(p1_), p2(p2_) {}

        inline TVec3 getCenter() const { return (p1 + p2) * 0.5; }
    };
}  // namespace tr

#endif  // TRAPDOOR_VEC3_H
