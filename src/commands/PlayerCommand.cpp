#include "CommandHelper.h"
#include "DynamicCommandAPI.h"

namespace tr {

    void setup_playerCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand(
            "player", "sapwn sim player",
            static_cast<CommandPermissionLevel>(level));

        auto playerOpt = command->setEnum("player opt", {"spawn", "despawn"});
        auto actionOpt = command->setEnum("player action", {"attack", "use"});
        auto status1Opt =
            command->setEnum("action Status", {"once", "continue"});
        //     auto status2Opt = command->setEnum("player action", {"attack",
        //     "use"});

        command->mandatory("player", ParamType::Enum, playerOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, actionOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("player", ParamType::Enum, status1Opt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        // command->mandatory("player", ParamType::Enum, status2Opt,
        //                    CommandParameterOption::EnumAutocompleteExpansion);

        // command->mandatory("player", ParamType::Player);
        command->mandatory("playerName", ParamType::String);
        command->optional("extra", ParamType::Int);
        //  command->addOverload({playerOpt, "player"});
        command->addOverload({"playerName", playerOpt});
        command->addOverload({"playerName", actionOpt, status1Opt, "extra"});

        //   command->addOverload({"playerName", actionOpt, status2Opt,
        //   "gametick"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {};
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr
