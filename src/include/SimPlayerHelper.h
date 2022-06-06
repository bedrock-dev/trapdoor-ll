#ifndef TRAPDOOR_SIM_PLAYER_H
#define TRAPDOOR_SIM_PLAYER_H
#include <MC/SimulatedPlayer.hpp>
#include <unordered_map>
#include <unordered_set>

#include "CommandHelper.h"

namespace tr {

    enum SimTaskType { Ineract, Destroy, UseOn, Use, Attack, Jump };

    struct SimTask {
        size_t tick;
        SimTaskType type;
        int interval = 0;
        int repteat_time = 0;
        SimulatedPlayer* player;
        Vec3 vec3;
        Actor* actor;
    };

    class SimPlayerManager {
       public:
        ActionResult useItemOnBlock(const std::string& name, int slot,
                                    const BlockPos& p, Player* ori);

        ActionResult useItemOnSlot(const std::string& name, int slot);

        // ActionResult setItem(const std::string& name);

        ActionResult getBagpack(const std::string& name, int slot);

        ActionResult destroy(const std::string& name, const BlockPos& p,
                             Player* origin);

        ActionResult interact(const std::string& name, Player* origin);

        ActionResult behavior(const std::string& name,
                              const std::string& behType, const Vec3& vec);

        ActionResult addPlayer(const std::string& name, const BlockPos& p,
                               int dimID);

        ActionResult addTask(const std::string& name, SimTaskType type,
                             const Vec3* vec3, Actor* target, int interval,
                             int repeatTime);

        ActionResult removePlayer(const std::string& name);

        ActionResult actionPlayer(const std::string& name,
                                  const std::string& action,
                                  const std::string& type, int extraAgrs);

        void tick();

       private:
        std::unordered_map<std::string, SimulatedPlayer*> players;

        std::queue<SimTask> taskqueue;
    };
}  // namespace tr

#endif