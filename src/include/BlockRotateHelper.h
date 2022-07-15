#ifndef TRAPDOOR_BLOCK_ROTATE_H
#define TRAPDOOR_BLOCK_ROTATE_H

// clang-format off
#include "Global.h"
#include <MC/BlockPos.hpp>
#include <MC/BlockSource.hpp>
#include <MC/BlockInstance.hpp>
#include <MC/Vec3.hpp>
// clang-format on

namespace trapdoor {
    void rotateBlock(BlockSource& bs, BlockInstance* bi, const Vec3& clickPos, unsigned char face);

}  // namespace trapdoor

#endif