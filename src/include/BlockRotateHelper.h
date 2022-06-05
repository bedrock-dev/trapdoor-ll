#ifndef TRAPDOOR_BLOCK_ROTATE_H
#define TRAPDOOR_BLOCK_ROTATE_H

// clang-format off
#include "Global.h"
#include <MC/BlockPos.hpp>
#include <MC/BlockSource.hpp>
// clang-format on

namespace tr {
    void rotateBlock(BlockSource &bs, const BlockPos &pos);
}

#endif