#ifndef _TRAPDOOR_TRAPDOOR_H_
#define _TRAPDOOR_TRAPDOOR_H_

#include "LoggerAPI.h"

namespace tr {
    Logger &logger();

    void InitMod();

    void setupCommands();
} // namespace tr

#endif