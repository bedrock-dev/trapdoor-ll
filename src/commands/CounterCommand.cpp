//
// Created by xhy on 2022/5/17.
//

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "SpawnHelper.h"
namespace tr {

    void setup_counterCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand(
            "counter", "hopper counter command",
            static_cast<CommandPermissionLevel>(level));

        auto &printOpt = command->setEnum("printCmd", {"print"});
        auto &resetOpt = command->setEnum("resetCmd", {"reset"});
        command->mandatory("counter", ParamType::Enum, printOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("counter", ParamType::Enum, resetOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->optional("channel", ParamType::Int);
        command->addOverload({printOpt, "channel"});
        command->addOverload({resetOpt, "channel"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            auto countParam = std::string();
            switch (do_hash(results["counter"].getRaw<std::string>().c_str())) {
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr