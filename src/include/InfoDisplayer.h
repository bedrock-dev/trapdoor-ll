#ifndef TRAPDOOR_INFO_DISPLAYER
#define TRAPDOOR_INFO_DISPLAYER

#include <MC/BlockInstance.hpp>
#include <MC/Dimension.hpp>

#include "Global.h"

namespace tr {

    bool displayActorInfo(Player *player, Actor *target);

    bool displayerBlockInfo(Player *p, BlockInstance *bi);
    bool displayerEnvInfo();

    bool displayerRedstoneCompInfo(Player *p, BlockInstance *bi);

}  // namespace tr

#endif