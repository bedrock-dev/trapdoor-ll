//
// Created by xhy on 2022/5/17.
//

#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "MCTick.h"
namespace trapdoor {
    void setup_tickCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("tick", "Change world ticking speed",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &optForward = command->setEnum("forward", {"forward", "fw", "warp"});
        command->mandatory("tick", ParamType::Enum, optForward,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tickNumber", ParamType::Int);
        command->addOverload({optForward, "tickNumber"});

        auto &optSpeedChange = command->setEnum("speedChange", {"acc", "slow"});
        command->mandatory("tick", ParamType::Enum, optSpeedChange,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("times", ParamType::Int);
        command->addOverload({optSpeedChange, "times"});

        auto &optFreeze = command->setEnum("freeze", {"freeze", "fz", "reset", "r", "query"});
        command->mandatory("tick", ParamType::Enum, optFreeze,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optFreeze});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            if (origin.getOriginType() == 1) {
                output.error("Tick command cannot be executed inside a command block");
                return;
            }

            switch (do_hash(results["tick"].getRaw<std::string>().c_str())) {
                case do_hash("acc"):
                    trapdoor::accWorld(results["times"].getRaw<int>()).sendTo(output);
                    break;
                case do_hash("slow"):
                    trapdoor::slowDownWorld(results["times"].getRaw<int>()).sendTo(output);
                    break;
                case do_hash("forward"):
                    // trapdoor::forwardWorld(results["tickNumber"].getRaw<int>()).sendTo(output);
                    // break;
                case do_hash("fw"):
                    trapdoor::forwardWorld(results["tickNumber"].getRaw<int>()).sendTo(output);
                    break;
                case do_hash("warp"):
                    trapdoor::warpWorld(results["tickNumber"].getRaw<int>()).sendTo(output);
                    break;
                case do_hash("freeze"):
                    // trapdoor::freezeWorld().sendTo(output);
                    // break;
                case do_hash("fz"):
                    trapdoor::freezeWorld().sendTo(output);
                    break;
                case do_hash("reset"):
                    // trapdoor::resetWorld().sendTo(output);
                    // break;
                case do_hash("r"):
                    trapdoor::resetWorld().sendTo(output);
                    break;
                case do_hash("query"):
                    trapdoor::queryWorld().sendTo(output);
                default:
                    break;
            }
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor