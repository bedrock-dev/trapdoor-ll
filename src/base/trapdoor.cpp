#include "trapdoor.h"
#include "LoggerAPI.h"
#include "commands.h"

namespace tr {

    Logger &logger() {
        static Logger logger("Trapdoor");
        return logger;
    }

    void InitMod() {
        setupCommands();
    }

    void setupCommands() {
        setupTickCommand();
    }
} // namespace tr
