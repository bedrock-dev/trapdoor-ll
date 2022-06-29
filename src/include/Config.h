//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_SETTINGS_H
#define TRAPDOOR_SETTINGS_H

#include <map>
#include <unordered_map>

#include "GlobalServiceAPI.h"
#include "MC/Command.hpp"
#include "MC/Level.hpp"
#include "MC/ServerPlayer.hpp"
#include "Nlohmann/json.hpp"
#include "Shortcuts.h"

namespace tr {
    struct CommandConfig {
        bool enable = true;
        int permissionLevel = 1;
    };
    struct BasicConfig {
        int particleLevel = 2;
        int particleViewDistance = 128;
        int hudRefreshFreq = 20;
    };

    struct DefaultEnableFunctions {
        bool blockRotate = false;
        bool hopperCounter = false;
        bool hud = false;
    };

    struct TweakConfig {
        int forcePlaceLevel = 0;
    };

    class Configuration {
       public:
        CommandConfig getCommandConfig(const std::string& command);
        inline std::vector<Shortcut>& getShortcuts() { return this->shortcuts; }
        inline BasicConfig& getBasicConfig() { return this->basicConfig; }
        inline TweakConfig& getTweakConfig() { return this->tweakConfig; }

        bool init(const std::string& fileName);

       private:
        bool readConfigFile(const std::string& path);

        bool readBasicConfigs();

        bool readCommandConfigs();

        bool readShortcutConfigs();

        bool readDefaultEnableFunctions();

        BasicConfig basicConfig;
        TweakConfig tweakConfig;
        std::unordered_map<std::string, CommandConfig> commandsConfigs;
        std::vector<Shortcut> shortcuts;
        nlohmann::json config;
    };

}  // namespace tr

#endif  // TRAPDOOR_SETTINGS_H
