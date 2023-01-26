#include "TrapdoorMod.h"

#include <cstdlib>
#include <mc/MapItem.hpp>

#include "BlockRotateHelper.h"
#include "CommandHelper.h"
#include "Events.h"
#include "LoggerAPI.h"
#include "SysInfoHelper.h"
#include "config.h"
#include "I18nAPI.h"

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
            REG_COMMAND(dis)
            REG_COMMAND(slime)
            REG_COMMAND(trigger)
            // REG_COMMAND(rand)
        }

        void printCopyrightInfo() {
            logger().info("");
            logger().info("Trapdoor : {}-{} Under AGPL License", STR(TRAPDOOR_VERSION),
                          STR(GAME_VERSION));
            logger().info("Build time: {}", STR(BUILD_TIME));
            logger().info(
                "Visit out github page for more info:  "
                "https://github.com/bedrock-dev/trapdoor-ll/");
            logger().info("");
        }

    }  // namespace

    void TrapdoorMod::heavyTick() {
        villageHelper.heavyTick();
        hsaManager.HeavyTick();
        HUDHelper.tick();
        slimeChunkHelper.HeavyTick();
    }
    void TrapdoorMod::lightTick() {
        villageHelper.lightTick();
        hopperChannelManager.tick();
        this->spawnAnalyzer.tick();
    }

    Logger &logger() {
        static Logger logger("trapdoor");
        return logger;
    }

    void TrapdoorMod::init() {
        printCopyrightInfo();
        this->initConfig(false);
        this->HUDHelper.init();
        trapdoor::initCPU();
        trapdoor::SubscribeEvents();
        trapdoor::initRotateBlockHelper();
        trapdoor::setupCommands();
    }

    bool TrapdoorMod::initConfig(bool reload) {
        auto modRootPath = std::string("./plugins/trapdoor/");
#ifdef DEV
        modRootPath = "C:/Users/xhy/dev/trapdoor-ll/src/base/";
        logger().consoleLevel = 8;
#endif
        logger().debug("mod root path: {}", modRootPath);
        Translation::load(modRootPath + "lang.json");
        return this->config.init(modRootPath + "config.json", reload);
    }

    void TrapdoorMod::getLevelNameFromUser() {
        //        if (!this->config.getBasicConfig().useClientLevel) {
        //            return;
        //        }
        //        auto homePath = std::string(getenv("HOMEPATH"));
        //        if (homePath.empty()) {
        //            trapdoor::logger().error("Can not get environment variable []");
        //        }
        //        trapdoor::logger().debug("User home path is {}", homePath);
        //        const std::string client_level_path;
        //
        //
    }

    TrapdoorMod &mod() {
        static TrapdoorMod mod;
        return mod;
    }
}  // namespace trapdoor
