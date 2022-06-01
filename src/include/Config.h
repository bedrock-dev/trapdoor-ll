//
// Created by xhy on 2022/5/17.
//

#ifndef TRAPDOOR_SETTINGS_H
#define TRAPDOOR_SETTINGS_H

#include <unordered_map>

#include "GlobalServiceAPI.h"
#include "MC/Command.hpp"
#include "MC/Level.hpp"
#include "MC/ServerPlayer.hpp"
#include "Nlohmann/json.hpp"

namespace tr {
    struct CommandConfig {
        bool enable = true;
        int permissionLevel = 1;
    };
    class Configuration {
        CommandConfig getCommmadConfig(const std::string& command);

        bool init(const std::string& fileName);

       private:
        bool readConfigFile(const std::string& path);

        bool readCommandConfigs();

        std::unordered_map<std::string, CommandConfig> commandsConfigs;
        nlohmann::json config;
    };
}  // namespace tr

#endif  // TRAPDOOR_SETTINGS_H
