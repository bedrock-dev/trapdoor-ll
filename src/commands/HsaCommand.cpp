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
        auto &cleanEnum = command->setEnum("cleanSubCommand", {"clear"});
        auto &placeEnum = command->setEnum("placeSubCommand", {"place"});
        command->mandatory("hsa", ParamType::Enum, showEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("hsa", ParamType::Enum, cleanEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("hsa", ParamType::Enum, placeEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("onoroff", ParamType::Bool);
        // command->mandatory("blockName", ParamType::Block);

        command->addOverload({showEnum, "onoroff"});
        command->addOverload({cleanEnum});
        command->addOverload({placeEnum, "blockName"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["hsa"].getRaw<std::string>().c_str())) {
                case do_hash("show"):
                    trapdoor::mod()
                        .getHsaManager()
                        .ShowHsa(results["onoroff"].getRaw<bool>())
                        .sendTo(output);
                    break;
                case do_hash("clear"):
                    trapdoor::mod().getHsaManager().clear().sendTo(output);
                    break;
                case do_hash("place"):
                    // results["blockName"].get<Block>();
                    break;
            }
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor
