//
// Created by xhy on 2022/5/17.
//

#include "TBlockPos.h"

#include <ostream>
#include <random>

#include "TVec3.h"

namespace tr {

    bool TBlockPos::operator==(const TBlockPos &v) const {
        return x == v.x && y == v.y && z == v.z;
    }

    bool TBlockPos::operator!=(const TBlockPos &v) const {
        return x != v.x || y != v.y || z != v.z;
    }

    double TBlockPos::distanceTo(const TBlockPos &blockPos) const {
        return static_cast<double>(sqrt((blockPos.x - x) * (blockPos.x - x) +
                                        (blockPos.y - y) * (blockPos.y - y) +
                                        (blockPos.z - z) * (blockPos.z - z)));
    }

    std::ostream &operator<<(std::ostream &os, const TBlockPos &vec3) {
        os << "[" << vec3.x << "," << vec3.y << "," << vec3.z << "]";
        return os;
    }

    TVec3 TBlockPos::toVec3() const { return {x, y, z}; }

    std::string TBlockPos::toString() const {
        return "[" + std::to_string(x) + "," + std::to_string(y) + "," + std::to_string(z) + "]";
    }

    std::vector<TBlockPos> TBlockPos::getNeighbourPos() {
        return {{x + 1, y, z}, {x - 1, y, z}, {x, y + 1, z},
                {x, y - 1, z}, {x, y, z + 1}, {x, y, z - 1}};
    }

    std::vector<TBlockPos> TBlockPos::getPlainNeighbourPos() {
        return {{x + 1, y, z}, {x - 1, y, z}, {x, y, z + 1}, {x, y, z - 1}};
    }

    TBlockPos2 TBlockPos::InChunkOffset() const {
        auto newX = x % 16;
        auto newZ = z % 16;
        if (newX < 0) newX += 16;
        if (newZ < 0) newZ += 16;
        return {newX, newZ};
    }

    TBlockPos2 TBlockPos::toChunkPos() const {
        auto cx = x < 0 ? x - 15 : x;
        auto cz = z < 0 ? z - 15 : z;
        return {cx / 16, cz / 16};
    }

    bool TBlockPos::operator<(const TBlockPos &rhs) const {
        if (x < rhs.x) return true;
        if (rhs.x < x) return false;
        if (y < rhs.y) return true;
        if (rhs.y < y) return false;
        return z < rhs.z;
    }

    int TBlockPos::operator*(const TBlockPos &pos) const {
        return this->x * pos.x + this->y * pos.y + this->z * pos.z;
    }

    TBlockPos TBlockPos::operator+(const TBlockPos &pos) const {
        return {x + pos.x, y + pos.y, z + pos.z};
    }

    bool TBlockPos2::isSlimeChunk() const {
        auto seed = (x * 0x1f1f1f1fu) ^ (uint32_t)z;
        std::mt19937 mt(seed);
        return mt() % 10 == 0;
    }

    bool TBlockPos2::operator<(const TBlockPos2 &rhs) const {
        if (x < rhs.x) return true;
        if (rhs.x < x) return false;
        return z < rhs.z;
    }

    std::string TBlockPos2::toString() const {
        return "[" + std::to_string(x) + "," + std::to_string(z) + "]";
    }
    bool TBlockPos2::operator==(const TBlockPos2 &rhs) const { return x == rhs.x && z == rhs.z; }
    TBlockPos2 &TBlockPos2::operator=(const TBlockPos2 &rhs) = default;

    std::string facingToString(TFACING facing) {
        switch (facing) {
            case TFACING::POS_X:
                return "+x";
            case TFACING::NEG_X:
                return "-x";
            case TFACING::POS_Y:
                return "+y";
            case TFACING::POS_Z:
                return "+z";
            case TFACING::NEG_Y:
                return "-y";
            case TFACING::NEG_Z:
                return "-z";
            default:
                return "unknown";
        }
    }

    std::string facingToDirString(TFACING facing) {
        switch (facing) {
            case TFACING::POS_X:
                return "east";
            case TFACING::NEG_X:
                return "west";
            case TFACING::POS_Y:
                return "up";
            case TFACING::POS_Z:
                return "south";
            case TFACING::NEG_Y:
                return "down";
            case TFACING::NEG_Z:
                return "north";
            default:
                return "unknown";
        }
    }

    TBlockPos facingToBlockPos(TFACING facing) {
        switch (facing) {
            case TFACING::NEG_Y:
                return {0, -1, 0};
            case TFACING::POS_Y:
                return {0, 1, 0};
            case TFACING::NEG_Z:
                return {0, 0, -1};
            case TFACING::POS_Z:
                return {0, 0, 1};
            case TFACING::NEG_X:
                return {-1, 0, 0};
            case TFACING::POS_X:
                return {1, 0, 0};
        }
        return {0, 0, 0};
    }

    bool facingIsPos(TFACING facing) {
        return facing == TFACING::POS_X || facing == TFACING::POS_Y || facing == TFACING::POS_Z;
    }

    bool facingIsNeg(TFACING facing) {
        return facing == TFACING::NEG_X || facing == TFACING::NEG_Y || facing == TFACING::NEG_Z;
    }

    bool facingIsX(TFACING facing) { return facing == TFACING::POS_X || facing == TFACING::NEG_X; }

    bool facingIsY(TFACING facing) { return facing == TFACING::POS_Y || facing == TFACING::NEG_Y; }

    bool facingIsZ(TFACING facing) { return facing == TFACING::POS_Z || facing == TFACING::NEG_Z; }

    TFACING invFacing(TFACING facing) {
        switch (facing) {
            case TFACING::NEG_Y:
                return TFACING::POS_Y;
            case TFACING::POS_Y:
                return TFACING::NEG_Y;
            case TFACING::NEG_Z:
                return TFACING::POS_Z;
            case TFACING::POS_Z:
                return TFACING::NEG_Z;
            case TFACING::NEG_X:
                return TFACING::POS_X;
            case TFACING::POS_X:
                return TFACING::NEG_X;
            default:
                return TFACING::POS_X;
        }
    }

}  // namespace tr