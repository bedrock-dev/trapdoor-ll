#include "TrapdoorMod.h"

#include "CommandHelper.h"
#include "LoggerAPI.h"

namespace tr {
    void TrapdoorMod::HeavyTick() { village_helper_.HeavyTick(); }
    void TrapdoorMod::LightTick() { village_helper_.LightTick(); }
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
        SetupTestCommand();
        SetupVillageCommand();
        SetupProfCommand();
        SetupLogCommand();
    }

}  // namespace tr
