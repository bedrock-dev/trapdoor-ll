#include "TrapdoorMod.h"

#include "CommandHelper.h"
#include "LoggerAPI.h"

namespace tr {

    Logger &logger() {
        static Logger logger("Trapdoor");
        return logger;
    }

    TrapdoorMod &mod() {
        static TrapdoorMod mod;
        return mod;
    }

    void InitMod() {
        logger().consoleLevel = 8;
        setupCommands();
    }

    void setupCommands() {
        SetupTickCommand();
        SetupVillageCommand();
        SetupProfCommand();
        SetupLogCommand();
    }

}  // namespace tr
