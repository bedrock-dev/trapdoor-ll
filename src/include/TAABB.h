#ifndef TRAPDOOR_TAABB_H
#define TRAPDOOR_TAABB_H

#include "TVec3.h"
namespace tr {
    struct TAABB {
        TVec3 p1{};
        TVec3 p2{};
        inline TAABB(const TVec3& pos1, const TVec3& pos2)
            : p1(pos1), p2(pos2) {}
    };

}  // namespace tr

#endif