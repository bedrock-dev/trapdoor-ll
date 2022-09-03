#ifndef PARTICLE_H

#define PARTICLE_H
#include "TAABB.h"
#include "TBlockPos.h"
#include "TVec3.h"

namespace trapdoor {
    enum PCOLOR {
        BLACK = 0,
        INDIGO = 1,
        LAVENDER = 2,
        TEAL = 3,
        COCOA = 4,
        DARK = 5,
        OATMEAL = 6,
        WHITE = 7,
        RED = 8,
        APRICOT = 9,
        YELLOW = 10,
        GREEN = 11,
        VATBLUE = 12,
        SLATE = 13,
        PINK = 14,
        FAWN = 15,
    };
    void spawnParticle(const TVec3& pos, const std::string& type, int dimID = 0);

    void drawLine(const TVec3& originPoint, TFACING direction, float length, PCOLOR color,
                  int dimType);

    void drawAABB(const TAABB& aabb, PCOLOR color, bool mark, int dimType);

    void shortHighlightBlock(const TBlockPos& pos, PCOLOR color, int dimType);

    void drawChunkSurface(const TBlockPos2&, int dimType);

    void spawnSlimeChunkParticle(const ChunkPos& p);

    void spawnNumParticle(const TVec3& v, int num, PCOLOR color, int dimType);

}  // namespace trapdoor

#endif