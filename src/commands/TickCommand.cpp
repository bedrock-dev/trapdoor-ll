//
// Created by xhy on 2022/5/17.
//

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include <string>

namespace tr {
    void SetupTickCommand() {

        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("tick", "change world tick command",
                                                     CommandPermissionLevel::GameMasters);


        auto &optForward = command->setEnum("forward", {"forward"});
        command->mandatory("tick", ParamType::Enum, optForward, CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tickNumber", ParamType::Int);
        command->addOverload({optForward, "tickNumber"});

        auto &optSpeedChange = command->setEnum("speedChange", {"accelerate", "slow"});
        command->mandatory("tick", ParamType::Enum, optSpeedChange, CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("times", ParamType::Int);
        command->addOverload({optSpeedChange, "times"});

        auto &optFreeze = command->setEnum("freeze", {"freeze", "reset", "query"});
        command->mandatory("tick", ParamType::Enum, optFreeze, CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optFreeze});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin, CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["tick"].getRaw<std::string>().c_str())) {
                case do_hash("accelerate"):
                    output.success(fmt::format("Acc {} times", results["times"].getRaw<int>()));
                    break;
                case do_hash("slow"):
                    output.success(fmt::format("slow {} times", results["times"].getRaw<int>()));
                    break;
                case do_hash("forward"):
                    output.success(fmt::format("Remove - {}", results["tickNumber"].getRaw<int>()));
                    break;
                case do_hash("freeze"):
                    output.success("success");
                default:
                    break;
            }
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}