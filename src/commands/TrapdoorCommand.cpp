//
// Created by xhy on 2022/5/17.
//

#include <unordered_map>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "TrapdoorMod.h"
namespace tr {

    namespace {
        ActionResult setParticlePerformanceLevel(const std::string &level) {
            auto &cfg = tr::mod().getConfig().getBasicConfig();
            if (level == "low") {
                cfg.particleLevel = 1;
                return {"Success", true};
            } else if (level == "medium") {
                cfg.particleLevel = 2;
                return {"Success", true};
            } else if (level == "high") {
                cfg.particleLevel = 3;
                return {"Success", true};
            }
            return {"Invalid value", false};
        }

        ActionResult setParticleViewDistance(int level) {
            if (level <= 0 || level > 4096) {
                return {"Invalid value,it should be within 1 to 4096", false};
            }
            tr::mod().getConfig().getBasicConfig().particleViewDistance = level;
            return {"Success", true};
        }

        ActionResult setHUDFreq(int freq) {
            if (freq <= 0) {
                return {"Invalid value, it should be with 0,+inf", false};
            }
            tr::mod().getConfig().getBasicConfig().hudRefreshFreq = freq;

            return {"Set freq to " + std::to_string(freq), true};
        }

    }  // namespace

    void setup_trapdoorCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        auto command = DynamicCommand::createCommand("trapdoor", "getConfig",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &particleShowLevelEnum = command->setEnum("particleShowLevelEnum", {"pm"});
        auto &particleShowLevelOpt = command->setEnum("level", {"low", "medium", "high"});
        auto &particleDistanceEnum = command->setEnum("particleDistanceEnum", {"pvd"});
        auto &hudRefreshFreqEnum = command->setEnum("hudRefreshFreq", {"hudfreq"});

        command->mandatory("trapdoor", ParamType::Enum, particleShowLevelEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("trapdoor", ParamType::Enum, particleDistanceEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("trapdoor", ParamType::Enum, hudRefreshFreqEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("particleLevelOpt", ParamType::Enum, particleShowLevelOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("maxDistance", ParamType::Int);
        command->mandatory("frequency", ParamType::Int);

        command->addOverload({particleShowLevelEnum, "particleLevelOpt"});
        command->addOverload({particleDistanceEnum, "maxDistance"});
        command->addOverload({hudRefreshFreqEnum, "frequency"});

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
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr
