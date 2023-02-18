//
// Created by xhy on 2022/5/17.
//

#include <processenv.h>

#include "BlockRotateHelper.h"
#include "CommandHelper.h"
#include "Configuration.h"
#include "Msg.h"
#include "Shortcuts.h"
#include "TrapdoorMod.h"

namespace trapdoor {

    namespace {

        void setIntValue(int &key, int value, const std::string &name, int min, int max) {
            if (value >= min && value <= max) {
                key = value;
                trapdoor::logger().debug("Set [{}] to {}", name, key);
            } else {
                trapdoor::logger().warn("Value of [{}] should within {} to {},set to default {}",
                                        name, min, max, key);
            }
        }

        void setBoolValue(bool &key, bool value, const std::string &name) {
            key = value;
            trapdoor::logger().debug(tr("Set [{}] to {}"), name, key);
        }

        template<typename T>
        std::string createItem(const std::string &name, T value) {
            trapdoor::TextBuilder builder;
            return builder.sText(TB::GRAY, " - ")
                    .text(name)
                    .text(":")
                    .sTextF(TB::GREEN | TB::BOLD, " %s\n", std::to_string(value).c_str())
                    .get();
        }

        std::string createStringItem(const std::string &name, const std::string &value) {
            trapdoor::TextBuilder builder;
            return builder.sText(TB::GRAY, " - ")
                    .text(name)
                    .text(":")
                    .sTextF(TB::GREEN | TB::BOLD, " %s\n", value.c_str())
                    .get();
        }

    }  // namespace

    bool Configuration::init(const std::string &fileName, bool reload) {
        if (!readConfigFile(fileName)) return false;

        if (!reload) {
            if (!readCommandConfigs()) return false;
        }

        if (!readBasicConfigs()) return false;
        if (!readShortcutConfigs()) return false;
        if (!readGlobalFunctionConfig()) return false;
        return true;
    }

    bool Configuration::readConfigFile(const std::string &path) {
        try {
            this->config.clear();
            std::ifstream i(path);
            i >> this->config;
            trapdoor::logger().info(tr("Read config file {} successfully"), path);
            return true;
        } catch (std::exception &) {
            trapdoor::logger().error(tr("Can't read config file in path {}"), path);
            return false;
        }
    }

    bool Configuration::readCommandConfigs() {
        auto cc = this->config["commands"];
        CommandConfig tempConfig;
        try {
            for (const auto &i: cc.items()) {
                const auto &value = i.value();
                tempConfig.enable = value["enable"].get<bool>();
                tempConfig.permissionLevel = value["permission-level"].get<int>();
                this->commandsConfigs.insert({i.key(), tempConfig});
            }
        } catch (const std::exception &e) {
            trapdoor::logger().error("error read commands: {}", e.what());
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
            auto tdh = bc["tool-damage-threshold"].get<int>();
            auto keepSimPlayerInv = bc["keep-sim-player-inv"].get<bool>();
            auto severCrashToken = bc["server-crash-token"].get<std::string>();
            auto botPrefix = bc["sim-player-prefix"].get<std::string>();
            auto useClientLevel = bc["use-client-level"].get<bool>();

            auto &cfg = this->basicConfig;

            setIntValue(cfg.particleLevel, pl, "particle performance level", 1, 3);
            setIntValue(cfg.particleViewDistance2D, pv * pv, "particle view distance", 0,
                        INT32_MAX);
            setIntValue(cfg.hudRefreshFreq, hudFreq, "hud refresh frequency", 1, 100000);
            setIntValue(cfg.toolDamageThreshold, tdh, "tool damage threshold", -100, 65536);
            setBoolValue(cfg.keepSimPlayerInv, keepSimPlayerInv, "keep sim player inv");
            setBoolValue(cfg.useClientLevel, useClientLevel, "use client's level");
            this->basicConfig.serverCrashToken = severCrashToken;
            this->basicConfig.simPlayerPrefix = botPrefix;

        } catch (const std::exception &e) {
            trapdoor::logger().error("error read basic-config: {}", e.what());
            return false;
        }
        return true;
    }

    bool Configuration::readShortcutConfigs() {
        try {
            this->shortcuts.clear();
            auto cc = this->config["shortcuts"];
            for (const auto &i: cc.items()) {
                Shortcut sh;
                const auto &value = i.value();
                auto type = value["type"].get<std::string>();
                auto actions = value["actions"];
                for (const auto &act: actions) {
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
                } else if (type == "destroy") {
                    sh.type = ShortcutType::DESTROY;
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
        } catch (const std::exception &e) {
            trapdoor::logger().error("error read shortcut getConfig: {}", e.what());
            return false;
        }
        return true;
    }

    CommandConfig Configuration::getCommandConfig(const std::string &command) {
        auto it = this->commandsConfigs.find(command);
        if (it == this->commandsConfigs.end()) {
            trapdoor::logger().warn("Can not find config info of [{}],it will not be registered",
                                    command);
            return {false, 2};
        }
        return it->second;
    }

    bool Configuration::readGlobalFunctionConfig() {
        /**
         *
    "[FOR GLOBAL]": "If the following settings are enabled then these features are enabled globally",
    "force-place-level": 0,
    "dropper-no-cost": false,
    "hopper-counter": false,
    "safe-explosion": false,
    "max-pending-tick-size": 100,
    "[FOR SINGLE]": "The following features are enabled only when both the following settings and the player's own settings(/self command) are enabled",
    "hud": true,
    "creative-no-clip": false,
    "block-rotate": false,
    "auto-select-tool": false,
    "force-open-container": false
         */

        try {

            auto d = this->config["default-functions-config"];
            auto forcePlaceLevel = d["force-place-level"].get<int>();
            auto dropperNoCost = d["dropper-no-cost"].get<bool>();
            auto hopperCounter = d["hopper-counter"].get<bool>();
            auto safeExplosion = d["safe-explosion"].get<bool>();
            auto maxPtSize = d["max-pending-tick-size"].get<int>();
            auto hud = d["hud"].get<bool>();
            auto creativeNoClip = d["creative-no-clip"].get<bool>();
            auto blockRotate = d["block-rotate"].get<bool>();
            auto autoSelectTool = d["auto-select-tool"].get<bool>();
            auto forceOpenContainer = d["force-open-container"].get<bool>();

            auto &cfg = this->globalFunctionConfig;

            setIntValue(cfg.forcePlaceLevel, forcePlaceLevel, "forcePlayerLevel", 0, 2);
            setBoolValue(cfg.dropperNoCost, dropperNoCost, "dropperNoCost");
            setBoolValue(cfg.hopperCounter, hopperCounter, "hopperCounter");
            setBoolValue(cfg.safeExplosion, safeExplosion, "Safe explosion");
            setIntValue(cfg.maxPendingTickSize, maxPtSize, "Max pending tick size", 1, 0xffffff);
            setBoolValue(cfg.hud, hud, "HUD");
            setBoolValue(cfg.creativeNoClip, creativeNoClip, "creative no clip");
            setBoolValue(cfg.blockRotate, blockRotate, "block rotate");
            setBoolValue(cfg.autoSelectTool, autoSelectTool, "auto select tool");
            setBoolValue(cfg.forceOpenContainer, forceOpenContainer, "force open container");

        } catch (const std::exception &e) {
            trapdoor::logger().error("Error read global function config: {}", e.what());
            return false;
        }

        return true;
    }


    std::string Configuration::dumpConfigInfo()  // NOLINT
    {

        TextBuilder builder;
        auto basicCfg = this->basicConfig;
        builder.sText(TB::BOLD | TB::WHITE, "Basic Configs:\n")

                .item("HUD refresh frequency", basicCfg.hudRefreshFreq)
                .item("Particle display level", basicCfg.particleLevel)
                .item("Particle view distance", sqrt(basicCfg.particleViewDistance2D))
                .item("Tool damage threshold", basicCfg.toolDamageThreshold)
                .item("Keep sim player Inv", basicCfg.keepSimPlayerInv)
                .item("Sim player name prefix", basicCfg.simPlayerPrefix);


        auto func = this->globalFunctionConfig;
        builder.sText(TB::BOLD | TB::WHITE, "Global Function Configs:\n")
                .item("Force place level", func.forcePlaceLevel)
                .item("Dropper no cost", func.dropperNoCost)
                .item("Hopper counter", func.hopperCounter)
                .item("Safe explosion", func.safeExplosion)
                .item("Max pending tick size ", func.maxPendingTickSize)
                        //For self
                .item("HUD", func.hud)
                .item("Creative no clip", func.creativeNoClip)
                .item("Block rotate", func.blockRotate)
                .item("Auto select tool", func.autoSelectTool)
                .item("Force open container", func.forceOpenContainer);

        builder.sText(TB::BOLD | TB::WHITE, "Shortcuts:\n");
        auto &scs = this->shortcuts;
        for (auto &sh: scs) {
            builder.sText(TB::GRAY, " - ").textF("%s\n", sh.getDescription().c_str());
        }
        return builder.get();
    }


}  // namespace trapdoor
