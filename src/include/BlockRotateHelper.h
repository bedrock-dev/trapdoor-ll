#ifndef TRAPDOOR_BLOCK_ROTATE_H
#define TRAPDOOR_BLOCK_ROTATE_H

// clang-format off
#include "Global.h"
#include <mc/BlockPos.hpp>
#include <mc/BlockSource.hpp>
#include <mc/BlockInstance.hpp>
#include <mc/Vec3.hpp>
#include "CommandHelper.h"
// clang-format on
namespace trapdoor {

    ActionResult setBlockRotationAble(bool able);

    bool rotateBlock(Player *p, BlockSource *bs, BlockInstance *bi, const Vec3 &clickPos, unsigned char face);

    void initRotateBlockHelper();
}  // namespace trapdoor

#endif