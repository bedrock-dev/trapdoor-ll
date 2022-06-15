//
// Created by xhy on 2022/5/17.
//

#include "Config.h"

#include "CommandHelper.h"
#include "Shortcuts.h"
#include "TrapdoorMod.h"
namespace tr {

    bool Configuration::init(const std::string& fileName) {
        if (!readConfigFile(fileName)) return false;
        if (!readCommandConfigs()) return false;
        if (!readShortcutConfigs()) return false;
        return true;
    }

    bool Configuration::readConfigFile(const std::string& path) {
        try {
            this->config.clear();
            std::ifstream i(path);
            i >> this->config;
            tr::logger().info("read getConfig file {} successfully", path);
            return true;
        } catch (std::exception&) {
            tr::logger().error("can not read getConfig file {}", path);
            return false;
        }
    }

    bool Configuration::readCommandConfigs() {
        auto cc = this->config["commands"];
        CommandConfig tempConfig;
        try {
            for (const auto& i : cc.items()) {
                const auto& value = i.value();
                tempConfig.enable = value["enable"].get<bool>();
                tempConfig.permissionLevel = value["permission-level"].get<int>();
                this->commandsConfigs.insert({i.key(), tempConfig});
            }
        } catch (const std::exception& e) {
            tr::logger().error("error read command getConfig: {}", e.what());
            return false;
        }
        return true;
    }

    bool Configuration::readShortcutConfigs() {
        auto cc = this->config["shortcuts"];
        Shortcut sh;
        try {
            for (const auto& i : cc.items()) {
                const auto& value = i.value();

                auto type = value["type"].get<std::string>();
                if (type == "use") {
                    sh.type = ShortcutType::USE;
                    sh.item = value["item"].get<std::string>();
                    sh.action = value["action"].get<std::string>();
                } else if (type == "use-on") {
                    sh.type = ShortcutType::USEON;
                    sh.item = value["item"].get<std::string>();
                    sh.block = value["block"].get<std::string>();
                    sh.action = value["action"].get<std::string>();
                } else if (type == "command") {
                    sh.type = ShortcutType::CMD;
                    sh.command = value["command"].get<std::string>();
                    sh.action = value["action"].get<std::string>();
                    registerShortcutCommand(sh.command, sh.action);
                } else {
                    tr::logger().warn("unknown shortcut type: {}", type);
                    continue;
                }
                if (sh.type != CMD) {
                    this->shortcutsConfigs.insert(sh);
                }
                tr::logger().debug("Read shortcut: {}", sh.getDescription());
            }
        } catch (const std::exception& e) {
            tr::logger().error("error read shortcut getConfig: {}", e.what());
            return false;
        }
        return true;
    }

    CommandConfig Configuration::getCommandConfig(const std::string& command) {
        auto it = this->commandsConfigs.find(command);
        if (it == this->commandsConfigs.end()) {
            tr::logger().warn("Can nod find getConfig info of [{}],it will not be registered",
                              command);
            return {false, 2};
        }
        return it->second;
    }

}  // namespace tr
