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
    class Configuration {
       public:
        CommandConfig getCommandConfig(const std::string& command);
        inline std::set<Shortcut>& getShortcuts() { return this->shortcutsConfigs; }
        bool init(const std::string& fileName);

       private:
        bool readConfigFile(const std::string& path);

        bool readCommandConfigs();

        bool readShortcutConfigs();

        std::unordered_map<std::string, CommandConfig> commandsConfigs;
        std::set<Shortcut> shortcutsConfigs;
        nlohmann::json config;
    };

}  // namespace tr

#endif  // TRAPDOOR_SETTINGS_H
