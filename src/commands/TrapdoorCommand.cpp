//
// Created by xhy on 2022/5/17.
//

#include <unordered_map>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"

namespace tr {
    void setup_trapdoorCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        auto command = DynamicCommand::createCommand(
            "trapdoor", "config", static_cast<CommandPermissionLevel>(level));

        auto &particleShowLevelEnum =
            command->setEnum("particleShowLevelEnum", {"pm"});
        auto &particleShowLevelOpt =
            command->setEnum("particleLevelenum", {"low", "medium", "high"});
        auto &particleDistanceEnum =
            command->setEnum("particleDistanceEnum", {"pvd"});

        command->mandatory("trapdoor", ParamType::Enum, particleShowLevelEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("trapdoor", ParamType::Enum, particleDistanceEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("particleLevelOpt", ParamType::Enum,
                           particleShowLevelOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("maxDistance", ParamType::Int);
        command->addOverload({particleShowLevelEnum, "particleLevelOpt"});
        command->addOverload({particleDistanceEnum, "maxDistance"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            switch (
                do_hash(results["trapdoor"].getRaw<std::string>().c_str())) {
                case do_hash("particleLevel"):
                    break;
                case do_hash("particleDitance"):
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr
