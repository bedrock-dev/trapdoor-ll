#ifndef _TRAPDOOR_TRAPDOOR_H_
#define _TRAPDOOR_TRAPDOOR_H_

#include "LoggerAPI.h"
#include "Settings.h"
#include "VillageHelper.h"
namespace tr {

    class TrapdoorMod {
        void HeavyTick();

        void LightTick();

        inline VillageHelper &village_helper() { return this->village_helper_; }

       private:
        VillageHelper village_helper_;
        Settings settings_;
    };

    Logger &logger();

    TrapdoorMod &mod();

    void InitMod();

    void setupCommands();

}  // namespace tr

#endif