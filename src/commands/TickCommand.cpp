//
// Created by xhy on 2022/5/17.
//

#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "MCTick.h"
namespace tr {

    void setup_tickCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("tick", "change world ticking speed",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &optForward = command->setEnum("forward", {"forward", "wrap"});
        command->mandatory("tick", ParamType::Enum, optForward,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tickNumber", ParamType::Int);
        command->addOverload({optForward, "tickNumber"});

        auto &optSpeedChange = command->setEnum("speedChange", {"acc", "slow"});
        command->mandatory("tick", ParamType::Enum, optSpeedChange,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("times", ParamType::Int);
        command->addOverload({optSpeedChange, "times"});

        auto &optFreeze = command->setEnum("freeze", {"freeze", "reset", "query"});
        command->mandatory("tick", ParamType::Enum, optFreeze,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optFreeze});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            if (origin.getOriginType() == 1) {
                output.error("tick command cannot be executed inside a command block");
                return;
            }

            switch (do_hash(results["tick"].getRaw<std::string>().c_str())) {
                case do_hash("acc"):
                    tr::accWorld(results["times"].getRaw<int>()).sendTo(output);
                    break;
                case do_hash("slow"):
                    tr::slowDownWorld(results["times"].getRaw<int>()).sendTo(output);
                    break;
                case do_hash("forward"):
                    tr::forwardWorld(results["tickNumber"].getRaw<int>()).sendTo(output);
                    break;
                case do_hash("wrap"):
                    tr::wrapWorld(results["tickNumber"].getRaw<int>()).sendTo(output);
                    break;
                case do_hash("freeze"):
                    tr::freezeWorld().sendTo(output);
                    break;
                case do_hash("reset"):
                    tr::resetWorld().sendTo(output);
                    break;
                case do_hash("query"):
                    tr::queryWorld().sendTo(output);
                default:
                    break;
            }
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace tr