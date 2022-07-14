#ifndef PARTICLE_H

#define PARTICLE_H
#include "TAABB.h"
#include "TBlockPos.h"
#include "TVec3.h"

namespace trapdoor {
    enum class PCOLOR { WHITE = 0, RED = 1, YELLOW = 2, BLUE = 3, GREEN = 4 };
    void spawnParticle(const TVec3& pos, const std::string& type, int dimID);

    void drawLine(const TVec3& originPoint, TFACING direction, float length, PCOLOR color,
                  int dimType);

    void drawAABB(const TAABB& aabb, PCOLOR color, bool mark, int dimType);

    void shortHighlightBlock(const TBlockPos& pos, PCOLOR color, int dimType);
}  // namespace trapdoor

#endif