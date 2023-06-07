#ifndef TRAPDOOR_TAABB_H
#define TRAPDOOR_TAABB_H

#include <string>

#include "TVec3.h"

namespace trapdoor {
    struct TAABB {
        TVec3 p1{};
        TVec3 p2{};
        inline TAABB(const TVec3& pos1, const TVec3& pos2) : p1(pos1), p2(pos2) {}

        [[nodiscard]] inline std::string ToString() const {
            return "[" + p1.toString() + ", " + p2.toString() + "]";
        }
    };

}  // namespace trapdoor

#endif