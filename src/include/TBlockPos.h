//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_BLOCKPOS_H
#define TRAPDOOR_BLOCKPOS_H

#include <string>
#include <vector>

namespace trapdoor {
    struct TVec3;
    struct TBlockPos2;
    struct TBlockPos;

    enum TFACING {
        NEG_Y = 0,
        POS_Y = 1,
        NEG_Z = 2,
        POS_Z = 3,
        NEG_X = 4,
        POS_X = 5,
    };

    bool facingIsPos(TFACING facing);

    bool facingIsNeg(TFACING facing);

    bool facingIsX(TFACING facing);

    bool facingIsY(TFACING facing);

    bool facingIsZ(TFACING facing);

    TFACING invFacing(TFACING facing);

    std::string facingToString(TFACING facing);

    std::string facingToDirString(TFACING facing);

    TBlockPos facingToBlockPos(TFACING facing);

    struct TBlockPos {
        int x = 0;
        int y = 0;
        int z = 0;

        TBlockPos() = default;

        TBlockPos(int _x, int _y, int _z) : x(_x), y(_y), z(_z) {}

        explicit TBlockPos(int x) : TBlockPos(x, x, x) {}

        explicit TBlockPos(float x) : TBlockPos(x, x, x) {}

        TBlockPos(float _x, float _y, float _z) : x((int)_x), y((int)_y), z((int)_z) {}

        bool operator==(const TBlockPos &v) const;

        bool operator!=(const TBlockPos &v) const;

        [[nodiscard]] double distanceTo(const TBlockPos &blockPos) const;

        friend std::ostream &operator<<(std::ostream &os, const TBlockPos &vec3);

        [[nodiscard]] TVec3 toVec3() const;

        [[nodiscard]] std::string toString() const;

        std::vector<TBlockPos> getNeighbourPos();

        std::vector<TBlockPos> getPlainNeighbourPos();

        [[nodiscard]] TBlockPos2 toChunkPos() const;

        [[nodiscard]] TBlockPos2 InChunkOffset() const;

        int operator*(const TBlockPos &pos) const;

        TBlockPos operator+(const TBlockPos &pos) const;

        bool operator<(const TBlockPos &rhs) const;
    };

    struct TBlockPos2 {
        int x = 0;
        int z = 0;

        TBlockPos2(int _x, int _z) : x(_x), z(_z) {}
        TBlockPos2() : TBlockPos2(0, 0) {}
        [[nodiscard]] std::string toString() const;

        [[nodiscard]] bool isSlimeChunk() const;

        bool operator<(const TBlockPos2 &rhs) const;

        TBlockPos2 &operator=(const TBlockPos2 &rhs);

        bool operator==(const TBlockPos2 &rhs) const;
    };

    // typedef TBlockPos2 ChunkPos;
    class TBoundingBox {
       public:
        TBlockPos minPos{};
        TBlockPos maxPos{};
        inline bool operator<(const TBoundingBox &rhs) const {
            if (minPos < rhs.minPos) return true;
            if (rhs.minPos < minPos) return false;
            return maxPos < rhs.maxPos;
        }
    };

}  // namespace trapdoor

#endif  // TRAPDOOR_BLOCKPOS_H
