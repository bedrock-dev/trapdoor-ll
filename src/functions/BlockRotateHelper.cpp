#include "BlockRotateHelper.h"

#include <MC/Block.hpp>
#include <MC/BlockLegacy.hpp>
#include <MC/BlockSource.hpp>
#include <regex>

#include "TrapdoorMod.h"
#include "Utils.h"
namespace trapdoor {
    // namespace

    void rotateBlock(BlockSource &bs, BlockInstance *bi, const Vec3 &clickPos, unsigned char face) {
        if (!bi || bi->isNull()) return;

        trapdoor::logger().debug("Block: {}, Position: {}, Click: {} Face: {}",
                                 bi->getBlock()->getTypeName(), bi->getPosition().toString(),
                                 clickPos.toString(), static_cast<int>(face));
        // TODO 写转方块的逻辑
    }
}  // namespace trapdoor
