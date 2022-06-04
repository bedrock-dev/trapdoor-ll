#include "BlockRotateHelper.h"

#include <MC/Block.hpp>
#include <MC/BlockLegacy.hpp>
#include <MC/BlockSource.hpp>

namespace tr {

    void rotateBlock(BlockSource &bs, const BlockPos &pos) {
        auto &block = bs.getBlock(pos);
        auto *n = block.getLegacyBlock().tryGetStateFromLegacyData(12);
        //  bs.setBlock(pos.x, pos.y, pos.z, *n, nullptr);
    }

}  // namespace tr