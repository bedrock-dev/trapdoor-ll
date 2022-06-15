#include "TrapdoorMod.h"

#include <MC/MapItem.hpp>

#include "CommandHelper.h"
#include "Config.h"
#include "Events.h"
#include "LoggerAPI.h"

#define REG_COMMAND(c)                                   \
    auto cfg_##c = cmdCfg.getCommandConfig(#c);          \
    if (cfg_##c.enable) {                                \
        tr::setup_##c##Command(cfg_##c.permissionLevel); \
        tr::logger().debug("register command " #c);      \
    }

namespace tr {
    namespace {
        void setupCommands() {
            auto &cmdCfg = tr::mod().getConfig();
            REG_COMMAND(spawn)
            REG_COMMAND(data)
            REG_COMMAND(trapdoor)
            REG_COMMAND(counter)
            REG_COMMAND(func)
            REG_COMMAND(tick)
            REG_COMMAND(test)
            REG_COMMAND(village)
            REG_COMMAND(prof)
            REG_COMMAND(log)
            REG_COMMAND(player)
            REG_COMMAND(hud)
        }

    }  // namespace

    void TrapdoorMod::heavyTick() {
        villageHelper.heavyTick();
        hsaManager.HeavyTick();
        HUDHelper.tick();
    }
    void TrapdoorMod::lightTick() {
        villageHelper.lightTick();
        hopperChannelManager.tick();
    }

    Logger &logger() {
        static Logger logger("Trapdoor");
        return logger;
    }

    void TrapdoorMod::init() {
        logger().consoleLevel = 8;
        // this->config.init(R"(C:\Users\xhy\dev\trapdoor-ll\src\base\config.json)");
        this->config.init(R"(config.json)");
        tr::SubscribeEvents();
        setupCommands();
    }

    TrapdoorMod &mod() {
        static TrapdoorMod mod;
        return mod;
    }

}  // namespace tr
