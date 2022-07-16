#include <MC/Player.hpp>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "HUDHelper.h"
#include "HsaHelper.h"
#include "TrapdoorMod.h"

namespace trapdoor {
    void setup_hudCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("hud", "getConfig hud display",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &modifyOpt = command->setEnum("modify", {"add", "remove"});
        auto &showOpt = command->setEnum("show", {"show"});

        command->mandatory("hud", ParamType::Enum, modifyOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("hud", ParamType::Enum, showOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        auto &showItemsOpt = command->setEnum(
            "showItems", {"base", "mspt", "hopper", "village", "redstone", "chunk"});

        command->mandatory("itemType", ParamType::Enum, showItemsOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("onoroff", ParamType::Bool);
        command->addOverload({modifyOpt, "itemType"});
        command->addOverload({showOpt, "onoroff"});
        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            // player needed
            if (!origin.getPlayer()) {
                ErrorPlayerNeed().sendTo(output);
                return;
            }

            switch (do_hash(results["hud"].getRaw<std::string>().c_str())) {
                case do_hash("add"):
                    trapdoor::mod()
                        .getHUDHelper()
                        .modifyPlayerInfo(
                            reinterpret_cast<Player *>(origin.getPlayer())->getRealName(),
                            results["itemType"].get<std::string>(), 1)
                        .sendTo(output);
                    break;
                case do_hash("remove"):
                    trapdoor::mod()
                        .getHUDHelper()
                        .modifyPlayerInfo(
                            reinterpret_cast<Player *>(origin.getPlayer())->getRealName(),
                            results["itemType"].get<std::string>(), 0)
                        .sendTo(output);
                    break;
                case do_hash("show"):
                    trapdoor::mod()
                        .getHUDHelper()
                        .setAblePlayer(
                            reinterpret_cast<Player *>(origin.getPlayer())->getRealName(),
                            results["onoroff"].get<bool>())
                        .sendTo(output);
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor