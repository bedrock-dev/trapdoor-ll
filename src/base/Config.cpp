//
// Created by xhy on 2022/5/17.
//

#include "Config.h"

#include "CommandHelper.h"
#include "Shortcuts.h"
#include "TrapdoorMod.h"
namespace trapdoor {
    bool Configuration::init(const std::string& fileName) {
        if (!readConfigFile(fileName)) return false;
        if (!readCommandConfigs()) return false;
        if (!readBasicConfigs()) return false;
        if (!readShortcutConfigs()) return false;
        if (!this->readDefaultEnableFunctions()) return false;
        return true;
    }

    bool Configuration::readConfigFile(const std::string& path) {
        try {
            this->config.clear();
            std::ifstream i(path);
            i >> this->config;
            trapdoor::logger().info("read getConfig file {} successfully", path);
            return true;
        } catch (std::exception&) {
            trapdoor::logger().error("can not read getConfig file {}", path);
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
            trapdoor::logger().error("error read command getConfig: {}", e.what());
            return false;
        }
        return true;
    }
    bool Configuration::readBasicConfigs() {
        try {
            auto bc = this->config["basic-config"];
            auto pl = bc["particle-performance-level"].get<int>();
            auto pv = bc["particle-view-distance"].get<int>();
            auto hudFreq = bc["hud-refresh-freq"].get<int>();

            if (pl != 1 && pl != 2 && pl != 3) {
                pl = 3;
                trapdoor::logger().warn("Invalid particle-performance-level, set to default 3");
            }
            this->basicConfig.particleLevel = pl;
            trapdoor::logger().debug("Set particle performance level to {}", pl);

            if (pv <= 0 || pv >= 2048) {
                pv = 128;
                trapdoor::logger().warn("Invalid particle-view-distance, set to default 128");
            }
            this->basicConfig.particleViewDistance = pv;
            trapdoor::logger().debug("Set particle view distance to {}", pv);

            if (hudFreq <= 0) {
                hudFreq = 20;
                trapdoor::logger().warn("Invalid hud refresh frequency, set to default 20");
            }
            this->basicConfig.hudRefreshFreq = hudFreq;
            trapdoor::logger().debug("Set HUD show freq to {}", hudFreq);

        } catch (const std::exception& e) {
            trapdoor::logger().error("error read  basic config: {}", e.what());
            return false;
        }
        return true;
    }
    bool Configuration::readShortcutConfigs() {
        try {
            auto cc = this->config["shortcuts"];
            for (const auto& i : cc.items()) {
                Shortcut sh;

                const auto& value = i.value();
                auto type = value["type"].get<std::string>();
                auto actions = value["actions"];
                for (const auto& act : actions) {
                    sh.actions.push_back(act.get<std::string>());
                }

                if (sh.actions.empty()) {
                    trapdoor::logger().error("Shortcut {} has no action", i.key());
                    continue;
                }

                if (type == "use") {
                    sh.type = ShortcutType::USE;
                    sh.setItem(value["item"].get<std::string>());
                    sh.prevent = value["prevent"].get<bool>();
                    this->shortcuts.push_back(sh);
                    trapdoor::logger().debug("Shortcut: {}", sh.getDescription());
                } else if (type == "use-on") {
                    sh.type = ShortcutType::USE_ON;
                    sh.setItem(value["item"].get<std::string>());
                    sh.setBlock(value["block"].get<std::string>());
                    sh.prevent = value["prevent"].get<bool>();
                    this->shortcuts.push_back(sh);
                    trapdoor::logger().debug("Shortcut: {}", sh.getDescription());
                } else if (type == "command") {
                    auto command = value["command"].get<std::string>();
                    registerShortcutCommand(command, actions);
                    continue;
                } else {
                    trapdoor::logger().error("unknown shortcut type: {}", type);
                }
            }
        } catch (const std::exception& e) {
            trapdoor::logger().error("error read shortcut getConfig: {}", e.what());
            return false;
        }
        return true;
    }

    CommandConfig Configuration::getCommandConfig(const std::string& command) {
        auto it = this->commandsConfigs.find(command);
        if (it == this->commandsConfigs.end()) {
            trapdoor::logger().warn("Can nod find config info of [{}],it will not be registered",
                                    command);
            return {false, 2};
        }
        return it->second;
    }
    bool Configuration::readDefaultEnableFunctions() {
        try {
            auto def = this->config["default-enable-functions"];
            auto& mod = trapdoor::mod();
            auto hc = def["hopper-counter"].get<bool>();
            // auto br = def["block-rotate"].get<bool>();
            auto hud = def["hud"].get<bool>();
            mod.getHopperChannelManager().setAble(hc);
            mod.getHUDHelper().setAble(hud);
            trapdoor::logger().warn("Set hopper counter to {}", hc);
            trapdoor::logger().warn("Set HUD to {}", hud);
            // trapdoor::logger().warn("Set block rotate to {}", br);
        } catch (const std::exception& e) {
            trapdoor::logger().error("error read shortcut getConfig: {}", e.what());
            return false;
        }

        return true;
    }

}  // namespace trapdoor
