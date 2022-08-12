//
// Created by xhy on 2022/8/12.
//
#include <MC/Player.hpp>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "EventTrigger.h"
#include "TrapdoorMod.h"

namespace trapdoor {
    void setup_triggerCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("trigger", "getConfig hud display",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &modifyOpt = command->setEnum("modify", {"sub", "unsub"});
        auto &listOpt = command->setEnum("show", {"list"});

        command->mandatory("trigger", ParamType::Enum, modifyOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("trigger", ParamType::Enum, listOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        auto &showItemsOpt = command->setEnum("showItems", trapdoor::collectTrEventsStr());

        command->mandatory("eventsType", ParamType::Enum, showItemsOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->addOverload({modifyOpt, "eventsType"});
        command->addOverload({listOpt});
        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            // player needed
            if (!origin.getPlayer()) {
                ErrorPlayerNeed().sendTo(output);
                return;
            }

            switch (do_hash(results["hud"].getRaw<std::string>().c_str())) {
                case do_hash("sub"):
                    // TODO
                    break;
                case do_hash("unsub"):
                    // TODO
                    break;
                case do_hash("list"):
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor