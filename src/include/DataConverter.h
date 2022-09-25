#ifndef DATA_CONVERTER_H
#define DATA_CONVERTER_H

// clang-format off
#include "Global.h"
#include <mc/AABB.hpp>
#include <mc/Vec3.hpp>
#include <mc/BlockPos.hpp>
#include "TAABB.h"
#include "TBlockPos.h"
// clang-format on

class AABB;
namespace trapdoor {
    // struct TVec3;
    // struct TAABB;
    // TVec3 fromVec3(const Vec3& vec3);
    // TAABB fromAABB(const AABB& aabb);
    inline TVec3 fromVec3(const Vec3& v) { return {v.x, v.y, v.z}; }

    inline TBlockPos fromBlockPos(const BlockPos& v) { return {v.x, v.y, v.z}; }

    inline TAABB fromAABB(const AABB& aabb) { return {fromVec3(aabb.min), fromVec3(aabb.max)}; }

}  // namespace trapdoor
#endif