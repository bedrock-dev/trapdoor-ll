//
// Created by xhy on 2022/5/17.
//

#include "BlockPos.h"
#include "Vec3.h"

#include <ostream>
#include <random>

namespace tr {
    bool BlockPos::operator==(const BlockPos &v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    bool BlockPos::operator!=(const BlockPos &v) const {
        return x != v.x || y != v.y || z != v.z;
    }

    double BlockPos::distanceTo(const BlockPos &blockPos) const {
        return static_cast<double >(sqrt((blockPos.x - x) * (blockPos.x - x) +
                                         (blockPos.y - y) * (blockPos.y - y) +
                                         (blockPos.z - z) * (blockPos.z - z)));
    }

    std::ostream &operator<<(std::ostream &os, const BlockPos &vec3) {
        os << "[" << vec3.x << "," << vec3.y << "," << vec3.z << "]";
        return os;
    }

    Vec3 BlockPos::toVec3() const { return {x, y, z}; }

    std::string BlockPos::toString() const {
        return "[" + std::to_string(x) + "," + std::to_string(y) + "," +
               std::to_string(z) + "]";
    }

    std::vector<BlockPos> BlockPos::getNeighbourPos() {
        return {{x + 1, y,     z},
                {x - 1, y,     z},
                {x,     y + 1, z},
                {x,     y - 1, z},
                {x,     y,     z + 1},
                {x,     y,     z - 1}};
    }

    std::vector<BlockPos> BlockPos::getPlainNeighbourPos() {
        return {{x + 1, y, z},
                {x - 1, y, z},
                {x,     y, z + 1},
                {x,     y, z - 1}};
    }

    BlockPos2 BlockPos::InChunkOffset() const {
        auto newX = x % 16;
        auto newZ = z % 16;
        if (newX < 0) newX += 16;
        if (newZ < 0) newZ += 16;
        return {newX, newZ};
    }

    BlockPos2 BlockPos::toChunkPos() const {
        auto cx = x < 0 ? x - 15 : x;
        auto cz = z < 0 ? z - 15 : z;
        return {cx / 16, cz / 16};
    }

    bool BlockPos::operator<(const BlockPos &rhs) const {
        if (x < rhs.x) return true;
        if (rhs.x < x) return false;
        if (y < rhs.y) return true;
        if (rhs.y < y) return false;
        return z < rhs.z;
    }

    int BlockPos::operator*(const BlockPos &pos) const {
        return this->x * pos.x + this->y * pos.y + this->z * pos.z;
    }

    BlockPos BlockPos::operator+(const BlockPos &pos) const {
        return {x + pos.x, y + pos.y, z + pos.z};
    }

    bool BlockPos2::isSlimeChunk() const {
        auto seed = (x * 0x1f1f1f1fu) ^ (uint32_t) z;
        std::mt19937 mt(seed);
        return mt() % 10 == 0;
    }

    bool BlockPos2::operator<(const BlockPos2 &rhs) const {
        if (x < rhs.x) return true;
        if (rhs.x < x) return false;
        return z < rhs.z;
    }
}