#ifndef _TRAPDOOR_TRAPDOOR_H_
#define _TRAPDOOR_TRAPDOOR_H_

#include "Config.h"
#include "EventTrigger.h"
#include "HUDHelper.h"
#include "HopperCounter.h"
#include "HsaHelper.h"
#include "LoggerAPI.h"
#include "SimPlayerHelper.h"
#include "SlimeChunkHelper.h"
#include "SpawnAnalyzer.h"
#include "VillageHelper.h"

namespace trapdoor {
    class TrapdoorMod {
       public:
        void init();

        bool initConfig(bool reload);

        void heavyTick();

        void lightTick();

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
    };

    Logger &logger();

    TrapdoorMod &mod();
}  // namespace trapdoor

#endif