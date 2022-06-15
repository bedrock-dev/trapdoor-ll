#include "BlockRotateHelper.h"

#include <MC/Block.hpp>
#include <MC/BlockLegacy.hpp>
#include <MC/BlockSource.hpp>

#include "TrapdoorMod.h"
namespace tr {

    void rotateBlock(BlockSource &bs, const BlockPos &pos) {
        tr::logger().debug("rotateBlock: {} {} {} ", pos.x,pos.y,pos.z);
        auto &block = bs.getBlock(pos);
        auto variant = block.getVariant();
        auto *newBlock = block.getLegacyBlock().tryGetStateFromLegacyData((variant + 1) % 16);
        bs.setBlock(pos.x, pos.y, pos.z, *newBlock, 3, nullptr);
    }
}  // namespace tr