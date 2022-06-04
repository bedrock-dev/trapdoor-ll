#include "TrapdoorMod.h"

#include "CommandHelper.h"
#include "Config.h"
#include "Events.h"
#include "LoggerAPI.h"

#define REG_COMMAND(c)                                   \
    auto cfg_##c = cmdCfg.getCommmadConfig(#c);          \
    if (cfg_##c.enable) {                                \
        tr::setup_##c##Command(cfg_##c.permissionLevel); \
        tr::logger().debug("register command " #c);      \
    }

namespace tr {
    namespace {
        void setupCommands() {
            auto &cmdCfg = tr::mod().config();
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
        }

    }  // namespace

    void TrapdoorMod::HeavyTick() {
        village_helper_.HeavyTick();
        hsa_magager_.HeavyTick();
        hud_helper_.tick();
    }
    void TrapdoorMod::LightTick() {
        village_helper_.LightTick();
        hopper_channel_manager_.tick();
    }

    Logger &logger() {
        static Logger logger("Trapdoor");
        return logger;
    }

    void TrapdoorMod::Init() {
        logger().consoleLevel = 8;
        this->config_.init(
            "C:\\Users\\xhy\\dev\\trapdoor-ll\\src\\base\\config.json");
        tr::SubscribeEvents();
        setupCommands();
    }

    TrapdoorMod &mod() {
        static TrapdoorMod mod;
        return mod;
    }

}  // namespace tr
