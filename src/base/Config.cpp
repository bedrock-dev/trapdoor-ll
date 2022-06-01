//
// Created by xhy on 2022/5/17.
//

#include "Config.h"

#include "TrapdoorMod.h"

namespace tr {

    bool Configuration::init(const std::string& fileName) {
        if (!readConfigFile(fileName)) return false;
        if (!readCommandConfigs()) return false;
    }

    bool Configuration::readConfigFile(const std::string& path) {
        try {
            this->config.clear();
            std::ifstream i(path);
            i >> this->config;
            tr::logger().info("read config file {} successfully", path);
            return true;
        } catch (std::exception& e) {
            tr::logger().error("can not read config file {}", path);
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
                tempConfig.permissionLevel =
                    value["permissionlevel"].get<int>();
                this->commandsConfigs.insert({i.key(), tempConfig});
            }
        } catch (const std::exception& e) {
            tr::logger().error("error read command config: {}", e.what());
            return false;
        }
        return true;
    }
    CommandConfig Configuration::getCommmadConfig(const std::string& command) {
        auto it = this->commandsConfigs.find(command);
        if (it == this->commandsConfigs.end()) {
            tr::logger().warn(
                "Can nod find config info of [{}],it will not be registered");
            return {false, 2};
        }
        return it->second;
    }

}  // namespace tr
