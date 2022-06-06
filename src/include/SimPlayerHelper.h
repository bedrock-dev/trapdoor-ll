#ifndef TRAPDOOR_SIM_PLAYER_H
#define TRAPDOOR_SIM_PLAYER_H
#include <MC/SimulatedPlayer.hpp>
#include <unordered_map>
#include <unordered_set>

#include "CommandHelper.h"

namespace tr {

    class SimPlayerManager {
       public:
        ActionResult destroy(const std::string& name, const BlockPos& p,
                             Player* origin);

        ActionResult interact(const std::string& name, Player* origin);

        ActionResult behavior(const std::string& name,
                              const std::string& behType, const Vec3& vec);

        ActionResult addPlayer(const std::string& name, const BlockPos& p,
                               int dimID);

        ActionResult removePlayer(const std::string& name);

        ActionResult actionPlayer(const std::string& name,
                                  const std::string& action,
                                  const std::string& type, int extraAgrs);

       private:
        std::unordered_map<std::string, SimulatedPlayer*> players;
    };
}  // namespace tr

#endif