#ifndef TRAPDOOR_TRAPDOOR_H
#define TRAPDOOR_TRAPDOOR_H

#include "Configuration.h"
#include "EventTrigger.h"
#include "HUDHelper.h"
#include "HopperCounter.h"
#include "HsaHelper.h"
#include "LoggerAPI.h"
#include "SimPlayerHelper.h"
#include "SlimeChunkHelper.h"
#include "SpawnAnalyzer.h"
#include "UserConfig.h"
#include "VillageHelper.h"
#include "Windows.h"

namespace trapdoor {
    class TrapdoorMod {
       public:
        void init();

        bool initConfig(bool reload);

        void getLevelNameFromUser();

        [[nodiscard]] std::string rootPath() const;

        void heavyTick();

        void lightTick();

        [[nodiscard]] std::string getVersionString() const;

        inline VillageHelper &getVillageHelper() { return this->villageHelper; }

        inline HsaManager &getHsaManager() { return this->hsaManager; }

        inline Configuration &getConfig() { return this->config; }

        inline HUDHelper &getHUDHelper() { return this->HUDHelper; }

        inline HopperChannelManager &getHopperChannelManager() {
            return this->hopperChannelManager;
        }

        inline SimPlayerManager &getSimPlayerManager() { return this->simPlayerManager; }

        inline SpawnAnalyzer &getSpawnAnalyzer() { return this->spawnAnalyzer; }

        inline SlimeChunkHelper &getSlimeChunkHelper() { return this->slimeChunkHelper; }

        inline EventTriggerManager &getEventTriggerMgr() { return this->eventTriggerManager; }

        inline UserConfig &getUserConfig() { return this->globalUserConfig; }

        inline std::unordered_map<std::string, const DynamicCommandInstance *>
            &getCommandInstanceMap() {
            return this->cmdInstanceMap;
        }

        const DynamicCommandInstance *getCmdInstance(const std::string &name);

       private:
        VillageHelper villageHelper;
        HsaManager hsaManager;
        Configuration config;
        HopperChannelManager hopperChannelManager;
        HUDHelper HUDHelper;
        EventTriggerManager eventTriggerManager;
        SimPlayerManager simPlayerManager;
        SpawnAnalyzer spawnAnalyzer;
        SlimeChunkHelper slimeChunkHelper;

        std::unordered_map<std::string, const DynamicCommandInstance *> cmdInstanceMap;

        // user config cache
        UserConfig globalUserConfig;

        // base path
        std::string baseLevelPath;
        std::string levelName;
        std::string modRootPath;
    };

    Logger &logger();

    TrapdoorMod &mod();
}  // namespace trapdoor

#endif