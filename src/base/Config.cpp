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
        try {
            for (const auto& i : cc.items()) {
                Shortcut sh;

                const auto& value = i.value();
                auto type = value["type"].get<std::string>();
                auto actions = value["actions"];
                for (const auto& act : actions) {
                    sh.actions.push_back(act.get<std::string>());
                }

                if (sh.actions.empty()) {
                    tr::logger().error("Shortcut {} has no action", i.key());
                    continue;
                }

                if (type == "use") {
                    sh.type = ShortcutType::USE;
                    sh.setItem(value["item"].get<std::string>());
                    sh.prevent = value["prevent"].get<bool>();
                    this->shortcuts.push_back(sh);
                    tr::logger().debug("Shortcut: {}", sh.getDescription());
                } else if (type == "use-on") {
                    sh.type = ShortcutType::USE_ON;
                    sh.setItem(value["item"].get<std::string>());
                    sh.setBlock(value["block"].get<std::string>());
                    sh.prevent = value["prevent"].get<bool>();
                    this->shortcuts.push_back(sh);
                    tr::logger().debug("Shortcut: {}", sh.getDescription());
                } else if (type == "command") {
                    auto command = value["command"].get<std::string>();
                    registerShortcutCommand(command, actions);
                    continue;
                } else {
                    tr::logger().error("unknown shortcut type: {}", type);
                }
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
