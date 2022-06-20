#ifndef TRAPDOOR_INFO_DISPLAYER
#define TRAPDOOR_INFO_DISPLAYER

#include <MC/BlockInstance.hpp>
#include <MC/BlockPos.hpp>
#include <MC/CircuitSceneGraph.hpp>
#include <MC/Dimension.hpp>
#include <MC/ServerPlayer.hpp>

#include "Global.h"

namespace tr {

    bool displayEntityInfo(Player *player, Actor *target);

    bool displayBlockInfo(Player *p, const BlockPos &pos);
    bool displayEnvInfo();

    bool displayRedstoneCompInfo(Player *p, const BlockPos &pos);

    CircuitSceneGraph &getCircuitSceneGraph(CircuitSystem *system);
}  // namespace tr

#endif