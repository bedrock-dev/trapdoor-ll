//
// Created by xhy on 2022/5/17.
//
#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "HsaHelper.h"
#include "TrapdoorMod.h"

namespace trapdoor {
    void setup_hsaCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        auto command = DynamicCommand::createCommand("hsa", "show hardcoded spawn area",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &showEnum = command->setEnum("showSubCommand", {"show"});
        auto &countEnum = command->setEnum("countSubCommand", {"count"});
        auto &placeEnum = command->setEnum("placeSubCommand", {"place"});
        command->mandatory("hsa", ParamType::Enum, showEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("hsa", ParamType::Enum, countEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("onoroff", ParamType::Bool);
        command->mandatory("hsa", ParamType::Enum, placeEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->optional("aironly", ParamType::Bool);
        command->addOverload({showEnum, "onoroff"});
        command->addOverload({countEnum});
        command->addOverload({placeEnum, "aironly"});
        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            bool airOnly = !results["aironly"].isSet || results["aironly"].getRaw<bool>();

            switch (do_hash(results["hsa"].getRaw<std::string>().c_str())) {
                case do_hash("show"):
                    trapdoor::mod()
                        .getHsaManager()
                        .ShowHsa(results["onoroff"].getRaw<bool>())
                        .sendTo(output);
                    break;
                case do_hash("count"):
                    trapdoor::mod().getHsaManager().count().sendTo(output);
                    break;
                case do_hash("place"):
                    trapdoor::mod()
                        .getHsaManager()
                        .place(origin.getPlayer(), airOnly)
                        .sendTo(output);
                    break;
            }
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor
