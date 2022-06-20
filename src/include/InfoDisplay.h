#ifndef TRAPDOOR_INFO_DISPLAYER
#define TRAPDOOR_INFO_DISPLAYER

#include <MC/BlockInstance.hpp>
#include <MC/BlockPos.hpp>
#include <MC/CircuitSceneGraph.hpp>
#include <MC/Dimension.hpp>
#include <MC/ServerPlayer.hpp>

#include "CommandHelper.h"
#include "Global.h"

namespace tr {

    bool displayEntityInfo(Player *player, Actor *target);

    ActionResult displayBlockInfo(Player *p, const BlockPos &pos, bool nbt,
                                  const std::string &path);
    bool displayEnvInfo();

    bool displayRedstoneCompInfo(Player *p, const BlockPos &pos);

}  // namespace tr

#endif