#include "TrapdoorMod.h"
#include "LoggerAPI.h"
#include "TrCommand.h"

namespace tr {

    Logger &logger() {
        static Logger logger("Trapdoor");
        return logger;
    }

    void InitMod() {
        setupCommands();
    }

    void setupCommands() {
        SetupTickCommand();
        SetupVillageCommand();
        SetupProfCommand();
    }

} // namespace tr
