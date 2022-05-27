
#include <MC/Dimension.hpp>
#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"

namespace tr {
    void SetupSpawnCommand() {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand(
            "spawn", "get spawn info", CommandPermissionLevel::GameMasters);

        // set enum就是给这个命令加一些enum值，不会产生任何意义
        auto &optCount =
            command->setEnum("actor counter sub command", {"count"});
        auto &optCountType =
            command->setEnum("counter options", {"chunk", "all", "density"});

        // mandatory/options就是给enum增加后置参数类型的,mandatory就是必填,optional是选填
        command->mandatory("spawn", ParamType::Enum, optCount,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("optCountType", ParamType::Enum, optCountType,
                           CommandParameterOption::EnumAutocompleteExpansion);

        // add
        // overload就是增加一些子命令，子命令需要Enum；并设定后面需要接什么类型的参数
        command->addOverload({optCount, "optCountType"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {};
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr