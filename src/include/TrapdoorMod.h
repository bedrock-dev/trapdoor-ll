#ifndef _TRAPDOOR_TRAPDOOR_H_
#define _TRAPDOOR_TRAPDOOR_H_

#include "Config.h"
#include "HUDHelper.h"
#include "HopperCounter.h"
#include "HsaHelper.h"
#include "LoggerAPI.h"
#include "VillageHelper.h"

namespace tr {

    class TrapdoorMod {
       public:
        void Init();

        void HeavyTick();

        void LightTick();

        inline VillageHelper &village_helper() { return this->village_helper_; }

        inline HsaManager &hsa_manager() { return this->hsa_magager_; }

        inline Configuration &config() { return this->config_; }

        inline HUDHelper &hud_helper() { return this->hud_helper_; }

        inline HopperChannelManager &hopper_channel_manager() {
            return this->hopper_channel_manager_;
        }

       private:
        VillageHelper village_helper_;
        HsaManager hsa_magager_;
        Configuration config_;
        HopperChannelManager hopper_channel_manager_;
        HUDHelper hud_helper_;
    };

    Logger &logger();

    TrapdoorMod &mod();
}  // namespace tr

#endif