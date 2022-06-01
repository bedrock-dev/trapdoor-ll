#ifndef _TRAPDOOR_TRAPDOOR_H_
#define _TRAPDOOR_TRAPDOOR_H_

#include "Config.h"
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

       private:
        VillageHelper village_helper_;
        HsaManager hsa_magager_;
        Configuration config_;
    };

    Logger &logger();

    TrapdoorMod &mod();
}  // namespace tr

#endif