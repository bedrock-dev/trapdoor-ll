#ifndef TRAPDOOR_SIM_PLAYER_H
#define TRAPDOOR_SIM_PLAYER_H
#include <unordered_set>

#include "CommandHelper.h"

namespace tr {

    class SimPlayerManager {
       public:
        ActionResult addPlayer(const std::string& name, const BlockPos& p,
                               int dimID);

       private:
        std::unordered_set<std::string> playerNames;
    };
}  // namespace tr

#endif