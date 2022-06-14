//
// Created by xhy on 2022/5/17.
//
#include <unordered_map>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "MCTick.h"

namespace tr {

    void setup_logCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("log", "print some world info",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &optMain = command->setEnum("main", {"mspt", "tps"});
        command->mandatory("log", ParamType::Enum, optMain,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optMain});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["log"].getRaw<std::string>().c_str())) {
                case do_hash("mspt"):
                    tr::printMSPT().sendTo(output);
                    break;
                default:
                    break;
            }
        };
        command->setCallback(cb);

        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr