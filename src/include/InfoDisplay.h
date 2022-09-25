#ifndef TRAPDOOR_INFO_DISPLAYER
#define TRAPDOOR_INFO_DISPLAYER

#include <mc/BlockInstance.hpp>
#include <mc/BlockPos.hpp>
#include <mc/CircuitSceneGraph.hpp>
#include <mc/Dimension.hpp>
#include <mc/ServerPlayer.hpp>

#include "CommandHelper.h"
#include "Global.h"

namespace trapdoor {
    ActionResult displayEntityInfo(Player *player, Actor *target, bool nbt,
                                   const std::string &path);

    ActionResult displayBlockInfo(Player *p, const BlockPos &pos, bool nbt,
                                  const std::string &path);
    bool displayEnvInfo();

}  // namespace trapdoor

#endif