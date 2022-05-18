#include "TrapdoorMod.h"
#include "LoggerAPI.h"
#include "CommandHelper.h"

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
        setupCommands();
    }

    void setupCommands() {
        SetupTickCommand();
        SetupVillageCommand();
        SetupProfCommand();
        SetupLogCommand();
    }


} // namespace tr
