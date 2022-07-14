#ifndef TRAPDOOR_BLOCK_ROTATE_H
#define TRAPDOOR_BLOCK_ROTATE_H

// clang-format off
#include "Global.h"
#include <MC/BlockPos.hpp>
#include <MC/BlockSource.hpp>
// clang-format on

namespace trapdoor {
    void initRotateBlockHelper();

    void rotateBlock(BlockSource & bs, const BlockPos &pos);
}  // namespace trapdoor

#endif