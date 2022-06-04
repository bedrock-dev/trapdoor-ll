#ifndef TRAPDOOR_BLOCK_ROTATE_H
#define TRAPDOOR_BLOCK_ROTATE_H
#include <MC/BlockPos.hpp>
#include <MC/BlockSource.hpp>

namespace tr {
    void rotateBlock(BlockSource &bs, const BlockPos &pos);
}

#endif