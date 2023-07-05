#include "TrapdoorMod.h"

#include <cstdlib>
#include <mc/MapItem.hpp>
#include <mc/SimulatedPlayer.hpp>

#include "BlockRotateHelper.h"
#include "ClientLevel.h"
#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "Events.h"
#include "I18nAPI.h"
#include "LoggerAPI.h"
#include "Msg.h"
#include "SysInfoHelper.h"
#include "TrapdoorAPI.h"
#include "config.h"

#define REG_COMMAND(c)                                             \
    auto cfg_##c = cmdCfg.getCommandConfig(#c);                    \
    if (cfg_##c.enable) {                                          \
        trapdoor::mod().getCommandInstanceMap()[#c] =              \
            trapdoor::setup_##c##Command(cfg_##c.permissionLevel); \
        trapdoor::logger().debug("Register command " #c);          \
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
            REG_COMMAND(dis)
            REG_COMMAND(slime)
            REG_COMMAND(trigger)
            REG_COMMAND(shortcut)
            REG_COMMAND(self)

            // REG_COMMAND(rand)
        }

        void printCopyrightInfo() {
            logger().info("");
            logger().info("Trapdoor : {}-{} Under AGPL License", STRING(TRAPDOOR_VERSION),
                          STRING(GAME_VERSION));
            logger().info("Build time: {}", STRING(BUILD_TIME));
            logger().info(
                "Visit our github page for more info:  "
                "https://github.com/bedrock-dev/trapdoor-ll/");
            logger().info("");
        }

        void tryCreateFolder(const std::string &folder) {
            namespace fs = std::filesystem;
            if (!fs::exists(folder)) {
                fs::create_directory(folder);
            }
        }

        void createFolders() {
            auto root = trapdoor::mod().rootPath();

            tryCreateFolder(root);
            tryCreateFolder(root + "./sim");
            tryCreateFolder(root + "./player");
            tryCreateFolder(root + "./logs");
            tryCreateFolder(root + "./scripts");
        }

    }  // namespace

    void TrapdoorMod::heavyTick() {
        villageHelper.heavyTick();
        hsaManager.HeavyTick();
        HUDHelper.tick();
        simPlayerManager.tick();
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
        this->initConfig(false);        // 读取配置文件
        this->globalUserConfig.init();  // 读取用户的配置

        trapdoor::setupClientLevel();
        trapdoor::initCPU();
        trapdoor::SubscribeEvents();
        trapdoor::initRotateBlockHelper();
        trapdoor::setupCommands();
        trapdoor::exportAPI();
    }

    bool TrapdoorMod::initConfig(bool reload) {
        this->modRootPath = std::string("./plugins/trapdoor/");
#ifdef DEV
        this->modRootPath = ::getenv("TR_ROOT_PATH");
        logger().consoleLevel = 8;
#endif
        logger().debug("Mod config path is : {}", modRootPath);
        createFolders();
        Translation::load(this->rootPath() + "lang.json");
        return this->config.init(this->rootPath() + "config.json", reload);
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

    std::string TrapdoorMod::getVersionString() const {
        trapdoor::TextBuilder tb;
        tb.sTextF(TB::BOLD | TB::AQUA, "Trapdoor v%s\n", STRING(TRAPDOOR_VERSION));
        tb.sTextF(TB::BOLD | TB::WHITE, "Developed by %s %s\n", "hhhxiao", "OEOTYAN");
        tb.textF("Build time: %s\n", STRING(BUILD_TIME));
        return tb.get();
    }

    std::string TrapdoorMod::rootPath() const { return this->modRootPath + "/"; }
    const DynamicCommandInstance *TrapdoorMod::getCmdInstance(const string &name) {
        auto it = this->cmdInstanceMap.find(name);
        if (it == this->cmdInstanceMap.end() || (!it->second)) {
            trapdoor::logger().debug("Can not find instance for command {}", name);
            return nullptr;
        }

        return it->second;
    }
    TrapdoorMod &mod() {
        static TrapdoorMod mod;
        return mod;
    }
}  // namespace trapdoor
