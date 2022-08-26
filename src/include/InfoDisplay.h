#ifndef TRAPDOOR_INFO_DISPLAYER
#define TRAPDOOR_INFO_DISPLAYER

#include <MC/BlockInstance.hpp>
#include <MC/BlockPos.hpp>
#include <MC/CircuitSceneGraph.hpp>
#include <MC/Dimension.hpp>
#include <MC/ServerPlayer.hpp>

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