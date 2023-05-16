//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_SETTINGS_H
#define TRAPDOOR_SETTINGS_H

#include <map>
#include <mc/Level.hpp>
#include <unordered_map>

#include "CommandHelper.h"
#include "Global.h"
#include "MC/ServerPlayer.hpp"
#include "Msg.h"
#include "Nlohmann/json.hpp"
#include "Shortcuts.h"
#include "UserConfig.h"

namespace trapdoor {
    struct CommandConfig {
        bool enable = true;
        int permissionLevel = 1;
    };

    struct BasicConfig {
        bool useClientLevel = false;
        int particleLevel = 2;
        int particleViewDistance2D = 4096;
        int hudRefreshFreq = 20;
        int toolDamageThreshold = 10;
        bool keepSimPlayerInv = true;
        std::string serverCrashToken;
        std::string simPlayerPrefix;
    };

    //    struct DefaultEnableFunctions {
    //        bool blockRotate = false;
    //        bool hopperCounter = false;
    //        bool hud = false;
    //    };

    /**


     "default-functions-config": {
    "hud": true,
    "creative-no-clip": false,
    "block-rotate": false,
    "auto-select-tool": false,
    "force-open-container": false,
    "force-place-level": 0,
    "dropper-no-cost": false,
    "hopper-counter": false,
    "safe-explosion": false,
    "max-pending-tick-size": 100
    },
    */

    // 所有功能的全局开关,后面不再在功能内部设定开关
    struct GlobalFunctionConfig {
        // 全局的
        int forcePlaceLevel = 0;
        bool dropperNoCost = false;
        bool hopperCounter = false;
        bool safeExplosion = false;
        bool disableNCUpdate = false;
        int maxPendingTickSize = 100;
        // 个人的
        bool hud = false;
        bool creativeNoClip = false;
        bool blockRotate = false;
        bool autoSelectTool = false;
        bool forceOpenContainer = false;
    };

    class Configuration {
       public:
        CommandConfig getCommandConfig(const std::string &command);

        inline std::unordered_map<std::string, Shortcut> &getShortcuts() { return this->shortcuts; }

        inline BasicConfig &getBasicConfig() { return this->basicConfig; }

        inline GlobalFunctionConfig &getGlobalFunctionConfig() {
            return this->globalFunctionConfig;
        }

        bool init(const std::string &fileName, bool reload);

        std::string dumpConfigInfo();

        template <typename T>
        void globalFunctionChangeListener(const std::string &key, T value) {
            trapdoor::broadcastMessage(trapdoor::SetValueMsg(key, value).msg, -1);
            if (key == "noclip") {
                Global<Level>->forEachPlayer([value](Player &p) {
                    if (!value) {  // 全局指令关闭，直接强制全部关闭
                        p.setAbility(static_cast<AbilitiesIndex>(17), false);
                    } else {  // 全局开启且自己开启 且是创造模式就自动更新 这里获取不到
                              // 不写了，需要玩家手动刷新
                        if (p.getPlayerGameType() == GameType::Creative) {
                            // TODO
                        }
                    };

                    return true;
                });
            }
            // other options
        }

        static std::vector<std::string> readBotScripts();

       private:
        bool readConfigFile(const std::string &path);

        bool readGlobalFunctionConfig();

        bool readBasicConfigs();

        bool readCommandConfigs();

        bool readShortcutConfigs();

        BasicConfig basicConfig;
        GlobalFunctionConfig globalFunctionConfig;
        std::unordered_map<std::string, CommandConfig> commandsConfigs;
        std::unordered_map<std::string, Shortcut> shortcuts;
        nlohmann::json config;
    };

}  // namespace trapdoor

#endif  // TRAPDOOR_SETTINGS_H
