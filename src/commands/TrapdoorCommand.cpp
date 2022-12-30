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
            trapdoor::mod().getConfig().getBasicConfig().particleViewDistance2D = level * level;
            return {"Success", true};
        }

        ActionResult setHUDFreq(int freq) {
            if (freq <= 0) {
                return {"Invalid value, it should be with 0,+inf", false};
            }
            trapdoor::mod().getConfig().getBasicConfig().hudRefreshFreq = freq;

            return {"Set freq to " + std::to_string(freq), true};
        }

        ActionResult reloadConfig() {
            auto succ = trapdoor::mod().initConfig(true);
            auto msg = succ ? "Success reload trapdoor config" : "Fail reload trapdoor config";
            return {msg, succ};
        }

        ActionResult crashServer(const std::string &token) {
            if (token == trapdoor::mod().getConfig().getBasicConfig().serverCrashToken) {
                //                Schedule::repeat(
                //                    []() {
                //                        static size_t counter = 10;
                //                        if (counter == 0) {
                //                            trapdoor::BroadcastMessage("Crashed!");
                //                            trapdoor::logger().warn("Server crashed");
                //                        }
                //                        auto msg = fmt::format("Sever will crash after {}
                //                        second(s)", counter); trapdoor::BroadcastMessage(msg, 1);
                //                        trapdoor::logger().warn(msg);
                //                        counter--;
                //                    },
                //                    20);
                trapdoor::logger().info("Crashed");
                abort();
            } else {
                return {"Error token", false};
            }
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
        auto &configEnum = command->setEnum("configEnum", {"dump", "reload"});
        auto &crashEnum = command->setEnum("crashEnum", {"crash"});

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

        command->mandatory("particleLevelOpt", ParamType::Enum, particleShowLevelOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("maxDistance", ParamType::Int);
        command->mandatory("frequency", ParamType::Int);
        command->mandatory("token", ParamType::String);

        command->addOverload({particleShowLevelEnum, "particleLevelOpt"});
        command->addOverload({particleDistanceEnum, "maxDistance"});
        command->addOverload({hudRefreshFreqEnum, "frequency"});
        command->addOverload({configEnum});
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
            }
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor
