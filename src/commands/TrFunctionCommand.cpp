//
// Created by xhy on 2022/5/17.
//
#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "SpawnHelper.h"
namespace tr {
    void SetupTrFunctionCommand() {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand(
            "func", "trapdoor functions", CommandPermissionLevel::GameMasters);

        auto &hoppercounterOpt =
            command->setEnum("hoppercounter", {"hoppercounter"});
        command->mandatory("func", ParamType::Enum, hoppercounterOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        auto &blockrotateOpt = command->setEnum("blockrotate", {"blockrotate"});
        command->mandatory("func", ParamType::Enum, blockrotateOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->optional("onoroff", ParamType::Int);
        command->addOverload({hoppercounterOpt, "onoroff"});
        command->addOverload({blockrotateOpt, "onoroff"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            auto countParam = std::string();
            switch (do_hash(results["func"].getRaw<std::string>().c_str())) {}
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace tr
