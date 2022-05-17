//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_BLOCKPOS_H
#define TRAPDOOR_BLOCKPOS_H


#include <string>
#include <vector>

namespace tr {
    struct Vec3;
    struct BlockPos2;

    struct BlockPos {
        int x = 0;
        int y = 0;
        int z = 0;

        BlockPos() = default;

        BlockPos(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

        explicit BlockPos(int x) : BlockPos(x, x, x) {}

        explicit BlockPos(float x) : BlockPos(x, x, x) {}

        BlockPos(float _x, float _y, float _z)
                : x((int) _x), y((int) _y), z((int) _z) {}

        bool operator==(const BlockPos &v) const;

        bool operator!=(const BlockPos &v) const;

        double distanceTo(const BlockPos &blockPos) const;

        friend std::ostream &operator<<(std::ostream &os, const BlockPos &vec3);

        Vec3 toVec3() const;

        std::string toString() const;

        std::vector<BlockPos> getNeighbourPos();

        std::vector<BlockPos> getPlainNeighbourPos();

        BlockPos2 toChunkPos() const;

        BlockPos2 InChunkOffset() const;

        int operator*(const BlockPos &pos) const;

        BlockPos operator+(const BlockPos &pos) const;

        bool operator<(const BlockPos &rhs) const;

    };

    struct BlockPos2 {
        int x = 0;
        int z = 0;

        BlockPos2(int _x, int _z) : x(_x), z(_z) {}

        std::string toString() const;

        bool isSlimeChunk() const;

        bool operator<(const BlockPos2 &rhs) const;
    };

    typedef BlockPos2 ChunkPos;


    class BoundingBox {
    };

    class ChunkPos {

    };
}


#endif //TRAPDOOR_BLOCKPOS_H
