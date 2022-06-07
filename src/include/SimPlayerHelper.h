#ifndef TRAPDOOR_SIM_PLAYER_H
#define TRAPDOOR_SIM_PLAYER_H
#include <MC/SimulatedPlayer.hpp>
#include <unordered_map>
#include <unordered_set>

#include "CommandHelper.h"
#include "ScheduleAPI.h"

namespace tr {

    enum SimTaskType { Ineract, Destroy, UseOn, Use, Attack, Jump };

    class SimPlayerManager {
       public:
        SimulatedPlayer* tryFetchSimPlayer(const std::string& name);

        ActionResult useItemOnBlock(const std::string& name, int slot,
                                    const BlockPos& p, Player* ori);

        ActionResult useItemOnSlot(const std::string& name, int slot);

        // ActionResult setItem(const std::string& name);

        ActionResult getBagpack(const std::string& name, int slot);

        //  ActionResult interact(const std::string& name, Player* origin);

        ActionResult behavior(const std::string& name,
                              const std::string& behType, const Vec3& vec);

        ActionResult addPlayer(const std::string& name, const BlockPos& p,
                               int dimID);

        ActionResult removePlayer(const std::string& name);

        ActionResult actionPlayer(const std::string& name,
                                  const std::string& action,
                                  const std::string& type, int extraAgrs);

        ActionResult interactSchedule(const std::string& name, Player* origin,
                                      int repType, int interval, int times);

        ActionResult destroySchedule(const std::string& name, const BlockPos& p,
                                     Player* origin, int repType, int interval,
                                     int times);

        void tick();

        void cancel(const std::string& name);

        bool checkSurvival(const std::string& name);

       private:
        std::unordered_map<std::string, SimulatedPlayer*> players;
        std::unordered_map<std::string, ScheduleTask> taskList;
    };
}  // namespace tr

#endif