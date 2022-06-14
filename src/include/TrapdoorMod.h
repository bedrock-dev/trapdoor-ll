#ifndef _TRAPDOOR_TRAPDOOR_H_
#define _TRAPDOOR_TRAPDOOR_H_

#include "Config.h"
#include "HUDHelper.h"
#include "HopperCounter.h"
#include "HsaHelper.h"
#include "LoggerAPI.h"
#include "SimPlayerHelper.h"
#include "VillageHelper.h"

namespace tr {

    class TrapdoorMod {
       public:
        void init();

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

       private:
        VillageHelper villageHelper;
        HsaManager hsaManager;
        Configuration config;
        HopperChannelManager hopperChannelManager;
        HUDHelper HUDHelper;
        SimPlayerManager simPlayerManager;
    };

    Logger &logger();

    TrapdoorMod &mod();
}  // namespace tr

#endif