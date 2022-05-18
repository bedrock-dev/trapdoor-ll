//
// Created by xhy on 2022/5/17.
//

#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "MCTick.h"
namespace tr {
    void SetupTickCommand() {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command =
            DynamicCommand::createCommand("tick", "change world tick command",
                                          CommandPermissionLevel::GameMasters);

        auto &optForward = command->setEnum("forward", {"forward"});
        command->mandatory("tick", ParamType::Enum, optForward,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tickNumber", ParamType::Int);
        command->addOverload({optForward, "tickNumber"});

        auto &optSpeedChange =
            command->setEnum("speedChange", {"accelerate", "slow"});
        command->mandatory("tick", ParamType::Enum, optSpeedChange,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("times", ParamType::Int);
        command->addOverload({optSpeedChange, "times"});

        auto &optFreeze =
            command->setEnum("freeze", {"freeze", "reset", "query"});
        command->mandatory("tick", ParamType::Enum, optFreeze,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optFreeze});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            switch (do_hash(results["tick"].getRaw<std::string>().c_str())) {
                case do_hash("accelerate"):
                    tr::AccWorld(results["times"].getRaw<int>()).SendTo(output);
                    break;
                case do_hash("slow"):
                    tr::SlowDownWorld(results["times"].getRaw<int>())
                        .SendTo(output);
                    break;
                case do_hash("forward"):
                    tr::ForwardWorld(results["tickNumber"].getRaw<int>())
                        .SendTo(output);
                    break;
                case do_hash("freeze"):
                    tr::FreezeWorld().SendTo(output);
                    break;
                case do_hash("reset"):
                    tr::ResetWorld().SendTo(output);
                    break;
                default:
                    break;
            }
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace tr