#ifndef TRAPDOOR_INFO_DISPLAYER
#define TRAPDOOR_INFO_DISPLAYER

#include <MC/BlockInstance.hpp>
#include <MC/Dimension.hpp>
#include <MC/ServerPlayer.hpp>

#include "Global.h"

namespace tr {

    bool displayEntityInfo(Player *player, Actor *target);

    bool displayerBlockInfo(Player *p, Block *bi);
    bool displayerEnvInfo();

    bool displayerRedstoneCompInfo(Player *p, Block *b);

}  // namespace tr

#endif