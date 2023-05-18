//
// Created by xhy on 2022/5/17.
//

#include <unordered_map>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "Msg.h"
#include "ScheduleAPI.h"
#include "TrapdoorMod.h"

namespace trapdoor {
    namespace {
        ActionResult setParticlePerformanceLevel(const std::string &level) {
            auto &cfg = trapdoor::mod().getConfig().getBasicConfig();
            bool success = true;
            if (level == "low") {
                cfg.particleLevel = 1;
            } else if (level == "medium") {
                cfg.particleLevel = 2;
            } else if (level == "high") {
                cfg.particleLevel = 3;
            } else {
                success = false;
            }
            return success ? trapdoor::OperationSuccess() : trapdoor::ErrorMsg("unknown error");
        }

        ActionResult setParticleViewDistance(int level) {
            if (level <= 0 || level > 4096) {
                return trapdoor::ErrorRange("Particle view distance", 0, 4096);
            }
            trapdoor::mod().getConfig().getBasicConfig().particleViewDistance2D = level * level;
            return trapdoor::OperationSuccess();
        }

        ActionResult setHUDFreq(int freq) {
            if (freq <= 0 || freq > 4096) {
                return trapdoor::ErrorRange("HUD refresh frequency", 0, 4096);
            }

            trapdoor::mod().getConfig().getBasicConfig().hudRefreshFreq = freq;
            return OperationSuccess();
        }

        ActionResult reloadConfig() {
            // trapdoor::mod().getConfig().readBotScripts(); //刷新脚本
            trapdoor::mod().getSimPlayerManager().refreshCommandScriptSoftEnum(
                trapdoor::Configuration::readBotScripts());
            return trapdoor::mod().initConfig(true)
                       ? trapdoor::SuccessMsg("trapdoor.reload.success")
                       : trapdoor::ErrorMsg("trapdoor.reload.error");
        }

        ActionResult crashServer(const std::string &token) {
            if (token == trapdoor::mod().getConfig().getBasicConfig().serverCrashToken) {
                trapdoor::logger().info("Crashed");
                abort();
            } else {
                return trapdoor::ErrorMsg("trapdoor.crash.error");
            }
        }

        ActionResult getModVersionInfo() {
            trapdoor::mod();
            return trapdoor::OperationSuccess();
        }

    }  // namespace

    const DynamicCommandInstance * setup_trapdoorCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        auto command = CREATE_CMD(trapdoor, level);

        auto &particleShowLevelEnum = command->setEnum("particleShowLevelEnum", {"pm"});
        auto &particleShowLevelOpt = command->setEnum("level", {"low", "medium", "high"});
        auto &particleDistanceEnum = command->setEnum("particleDistanceEnum", {"pvd"});
        auto &hudRefreshFreqEnum = command->setEnum("hudRefreshFreq", {"hudfreq"});
        auto &configEnum = command->setEnum("configEnum", {"dump", "reload"});
        auto &crashEnum = command->setEnum("crashEnum", {"crash"});
        auto &versionEnum = command->setEnum("versionEnum", {"version"});

        command->mandatory("trapdoor", ParamType::Enum, particleShowLevelEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("trapdoor", ParamType::Enum, particleDistanceEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("trapdoor", ParamType::Enum, hudRefreshFreqEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("trapdoor", ParamType::Enum, configEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("trapdoor", ParamType::Enum, crashEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("trapdoor", ParamType::Enum, versionEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("particleLevelOpt", ParamType::Enum, particleShowLevelOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("maxDistance", ParamType::Int);
        command->mandatory("frequency", ParamType::Int);
        command->mandatory("token", ParamType::String);

        command->addOverload({particleShowLevelEnum, "particleLevelOpt"});
        command->addOverload({particleDistanceEnum, "maxDistance"});
        command->addOverload({hudRefreshFreqEnum, "frequency"});
        command->addOverload({configEnum});
        command->addOverload({versionEnum});
        command->addOverload({crashEnum, "token"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["trapdoor"].getRaw<std::string>().c_str())) {
                case do_hash("pm"):
                    setParticlePerformanceLevel(results["particleLevelOpt"].getRaw<std::string>())
                        .sendTo(output);
                    break;
                case do_hash("pvd"):
                    setParticleViewDistance(results["maxDistance"].get<int>()).sendTo(output);
                    break;
                case do_hash("hudfreq"):
                    setHUDFreq(results["frequency"].get<int>()).sendTo(output);
                    break;
                case do_hash("reload"):
                    reloadConfig().sendTo(output);
                    break;
                case do_hash("dump"):
                    ActionResult(trapdoor::mod().getConfig().dumpConfigInfo(), true).sendTo(output);
                    break;
                case do_hash("crash"):
                    crashServer(results["token"].get<std::string>()).sendTo(output);
                    break;
                case do_hash("version"):
                    ActionResult(trapdoor::mod().getVersionString(), true).sendTo(output);
                    break;
            }
        };

        command->setCallback(cb);
      return  DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor
