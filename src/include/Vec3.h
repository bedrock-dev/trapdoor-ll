//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_VEC3_H
#define TRAPDOOR_VEC3_H

#include <string>

namespace tr {
    struct BlockPos;

    struct Vec3 {
        float x;
        float y;
        float z;

        Vec3() = default;

        explicit Vec3(int _x);

        explicit Vec3(float _x);

        Vec3(int _x, int _y, int _z)
                : x((float) _x), y((float) _y), z((float) _z) {}

        Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

        bool operator==(const Vec3 &v) const;

        bool operator!=(const Vec3 &v) const;

        bool operator<(const Vec3 &v) const;

        Vec3 operator+(const Vec3 &v) const;

        Vec3 operator-(const Vec3 &v) const;

        Vec3 operator*(float times) const;

        double distanceTo(const Vec3 &vec3) const;

        std::string toString() const;

        BlockPos toBlockPos() const;
    };

    class AABB {

    };
}


#endif //TRAPDOOR_VEC3_H
