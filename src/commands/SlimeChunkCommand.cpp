
#include <MC/Player.hpp>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "TrapdoorMod.h"

namespace trapdoor{
    void setup_slimeCommand(int level){
        using ParamType = DynamicCommand::ParameterType;
        auto command = DynamicCommand::createCommand("slime", "show slime spawn chunk",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &showEnum = command->setEnum("showSubCommand", {"show"});
        auto &cleanEnum = command->setEnum("cleanSubCommand", {"clear"});
        auto &rangeEnum = command->setEnum("rangeSubCommand", {"range"});
        command->mandatory("slime", ParamType::Enum, showEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("slime", ParamType::Enum, cleanEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("slime", ParamType::Enum, rangeEnum,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("onoroff", ParamType::Bool);
        command->mandatory("range", ParamType::Int);

        command->addOverload({showEnum, "onoroff"});
        command->addOverload({cleanEnum});
        command->addOverload({rangeEnum, "range"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            // player needed
            if (!origin.getPlayer()) {
                ErrorPlayerNeed().sendTo(output);
                return;
            }

            switch (do_hash(results["slime"].getRaw<std::string>().c_str())) {
                case do_hash("show"):
                    trapdoor::mod()
                        .getSlimeChunkHelper()
                        .ShowSlime(results["onoroff"].getRaw<bool>())
                        .sendTo(output);
                    break;
                case do_hash("clear"):
                    trapdoor::mod().getSlimeChunkHelper().updateChunkPosList();
                    trapdoor::mod().getSlimeChunkHelper().draw();
                    break;
                case do_hash("range"):
                    trapdoor::mod()
                        .getSlimeChunkHelper()
                        .setRadius(results["range"].getRaw<int>())
                        .sendTo(output);
                    break;
            }
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}