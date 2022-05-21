//
// Created by xhy on 2022/5/17.
//
#include "DynamicCommandAPI.h"
#include "VillageHelper.h"
namespace tr {
    void SetupVillageCommand() {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand(
            "village", "village helper", CommandPermissionLevel::GameMasters);

        auto &optSwitch = command->setEnum(
            "switch", {"bound", "spawn", "center", "poi", "head"});
        command->mandatory("village", ParamType::Enum, optSwitch,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("onOroff", ParamType::Bool);
        command->addOverload({optSwitch, "onOroff"});

        auto &optOther = command->setEnum("other", {"list", "near"});
        command->mandatory("village", ParamType::Enum, optOther,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optOther});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            switch (do_hash(results["village"].getRaw<std::string>().c_str())) {
                case do_hash("list"):
                    tr::printTickingVillages().SendTo(output);
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace tr