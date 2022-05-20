#ifndef _TRAPDOOR_TRAPDOOR_H_
#define _TRAPDOOR_TRAPDOOR_H_

#include "LoggerAPI.h"
#include "Settings.h"

namespace tr {

    class TrapdoorMod {
        Settings settings;
    };

    Logger &logger();

    TrapdoorMod &mod();

    void InitMod();

    void setupCommands();
}  // namespace tr

#endif