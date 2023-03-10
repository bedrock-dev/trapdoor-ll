#ifndef TRAPDOOR_SIM_PLAYER_H
#define TRAPDOOR_SIM_PLAYER_H

#include <mc/BlockSource.hpp>
#include <mc/SimulatedPlayer.hpp>
#include <unordered_map>
#include <unordered_set>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "ScheduleAPI.h"
#include "ScriptDriver.h"

namespace trapdoor {
    class SimPlayerManager {
        struct SimInfo {
            std::string name;
            SimulatedPlayer *simPlayer;
            ScheduleTask task;
            ScriptDriver driver;
        };

        void syncPlayerListToFile();

    public:
        inline void setupCommandInstance(const DynamicCommandInstance *instance) {
            this->cmdInstance = instance;
        }


        void savePlayerInventoryToFile();

        void addPlayersInCache();

        void tick();

        void processDieEvent(const std::string &name);

        SimulatedPlayer *tryFetchSimPlayer(const std::string &name, bool needFree);

        ActionResult listAll();

        ActionResult getBackpack(const std::string &name, int slot);

        ActionResult behavior(const std::string &name, const std::string &behType, const Vec3 &vec);

        ActionResult addPlayer(const std::string &name, const Vec3 &p, int dimID, int gameMode,
                               Player *origin);

        ActionResult removePlayer(const std::string &name);

        ActionResult interactSchedule(const std::string &name, Player *origin, int repType,
                                      int interval, int times);

        ActionResult attackSchedule(const std::string &name, Player *origin, int repType,
                                    int interval, int times);

        ActionResult destroyPositionSchedule(const std::string &name, const BlockPos &p, Player *origin,
                                             int repType, int interval, int times);

        ActionResult destroySchedule(const std::string &name, int repType, int interval, int times);

        ActionResult jumpSchedule(const std::string &name, int repType, int interval, int times);

        ActionResult useSchedule(const std::string &name, int itemId, int repType, int interval,
                                 int times);

        ActionResult useOnPositionSchedule(const std::string &name, int itemID, const BlockPos &p,
                                           Player *ori, int repType, int interval, int times);

        ActionResult
        useOnSchedule(const std::string &name, int itemID, Player *ori, int repType, int interval, int times);


        ActionResult runCmdSchedule(const std::string &name, const std::string &command,
                                    int repType, int interval, int times);

        ActionResult setItem(const std::string &name, int itemId, int slot);

        ActionResult dropItem(const std::string &name, int itemId);

        ActionResult dropAllItems(const std::string &name, int itemId);

        ActionResult followActor(const std::string &name, Player *player);

        ActionResult getSimPlayerInfo(const std::string &name);

        ActionResult teleportTo(const std::string &name, const Vec3 &position);

        ActionResult swapBackpack(const std::string &name, Player *origin);

        ActionResult runScript(const std::string &name, const std::string &scriptPath, int interval);

        void cancel(const std::string &name);

        void stopAction(const std::string &name);

        bool checkSurvival(const std::string &name);

    private:
        void refreshCommandSoftEnum();

        // static SimulatedPlayer *createSimPlayer(const std::string &name);

        std::unordered_map<std::string, SimInfo> simPlayers;
        const DynamicCommandInstance *cmdInstance = nullptr;
    };
}  // namespace trapdoor

#endif