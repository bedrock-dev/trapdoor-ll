#ifndef PARTICLE_H

#define PARTICLE_H
#include "TAABB.h"
#include "TBlockPos.h"
#include "TVec3.h"

namespace tr {

    enum class PCOLOR { WHITE = 0, RED = 1, YELLOW = 2, BLUE = 3, GREEN = 4 };
    void SpawnParticle(const TVec3& pos, const std::string& type, int dimID);

    void DrawLine(const TVec3& originPoint, TFACING direction, float length,
                  PCOLOR color, int dimType);

    void DrawAABB(const TAABB& aabb, PCOLOR color, bool mark, int dimType);
}  // namespace tr
#endif