#ifndef PARTICLE_H

#define PARTICLE_H
#include "TBlockPos.h"
#include "TVec3.h"

namespace tr {

    enum class PCOLOR { WHITE = 0, RED = 1, YELLOW = 2, BLUE = 3, GREEN = 4 };

    void DrawLine(const TVec3& originPoint, TFACING direction, float length,
                  PCOLOR color, int dimType);

}  // namespace tr
#endif