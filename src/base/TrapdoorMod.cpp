#include "TrapdoorMod.h"

#include <MC/MapItem.hpp>

#include "BlockRotateHelper.h"
#include "CommandHelper.h"
#include "Events.h"
#include "LoggerAPI.h"
#include "SysInfoHelper.h"
#define REG_COMMAND(c)                                         \
    auto cfg_##c = cmdCfg.getCommandConfig(#c);                \
    if (cfg_##c.enable) {                                      \
        trapdoor::setup_##c##Command(cfg_##c.permissionLevel); \
        trapdoor::logger().debug("register command " #c);      \
    }

namespace trapdoor {
    namespace {
        void setupCommands() {
            auto &cmdCfg = trapdoor::mod().getConfig();
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
            REG_COMMAND(hsa)
            REG_COMMAND(tweak)
            REG_COMMAND(seed)
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
        this->spawnAnalyzer.tick();
    }

    Logger &logger() {
        static Logger logger("Trapdoor");
        return logger;
    }

    void TrapdoorMod::init() {
        auto path = std::string("./plugins/trapdoor/");
#ifdef DEV
        path = "C:/Users/xhy/dev/trapdoor-ll/src/base/";
        logger().consoleLevel = 8;
#endif
        //  std::string path = "./plugins/trapdoor/";
        logger().debug("Config path: {}", path);
        this->config.init(path + "config.json");
        trapdoor::initCPU();
        trapdoor::SubscribeEvents();
        trapdoor::setupCommands();
    }

    TrapdoorMod &mod() {
        static TrapdoorMod mod;
        return mod;
    }
}  // namespace trapdoor
