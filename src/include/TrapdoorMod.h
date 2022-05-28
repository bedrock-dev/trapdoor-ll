#ifndef _TRAPDOOR_TRAPDOOR_H_
#define _TRAPDOOR_TRAPDOOR_H_

#include "HsaHelper.h"
#include "LoggerAPI.h"
#include "Settings.h"
#include "VillageHelper.h"

namespace tr {

    class TrapdoorMod {
       public:
        void HeavyTick();

        void LightTick();

        inline VillageHelper &village_helper() { return this->village_helper_; }

        inline HsaManager &hsa_manager() { return this->hsa_magager_; }

       private:
        VillageHelper village_helper_;
        HsaManager hsa_magager_;

        Settings settings_;
    };

    Logger &logger();

    TrapdoorMod &mod();

    void InitMod();

    void setupCommands();

}  // namespace tr

#endif