#ifndef DATA_CONVERTER_H
#define DATA_CONVERTER_H

#include "Global.h"

#include <MC/AABB.hpp>


//#include <MC/Vec3.hpp>

#include "TAABB.h"

class AABB;
namespace tr {

    // struct TVec3;
    // struct TAABB;
    // TVec3 fromVec3(const Vec3& vec3);
    // TAABB fromAABB(const AABB& aabb);
    // inline TVec3 ToTVec3(const Vec3& v) { return TVec3(v.x, v.y, v.z); }

    inline const TAABB* fromAABB(const AABB* aabb) {
        return reinterpret_cast<const TAABB*>(aabb);
    }

}  // namespace tr
#endif