//
// Created by xhy on 2022/5/17.
//
#include <mc/Player.hpp>
#include <mc/ServerPlayer.hpp>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"
namespace trapdoor {
    void registerShortcutCommand(const std::string &shortcut,
                                 const std::vector<std::string> &actions) {
        trapdoor::logger().debug("registerShortcutCommand: {} => ", shortcut);
        for (auto &action : actions) {
            trapdoor::logger().debug("        - {}", action);
        }

        using ParamType = DynamicCommand::ParameterType;
        auto description = "Shortcut for /" + actions[0] + " ...";
        auto command = DynamicCommand::createCommand(shortcut, description);
        command->addOverload(std::vector<std::string>{});
        auto cb = [actions](DynamicCommand const &command, CommandOrigin const &origin,
                            CommandOutput &output,
                            std::unordered_map<std::string, DynamicCommand::Result> &results) {
            auto *p = origin.getPlayer();
            if (p) {
                auto pos = p->getPos().toBlockPos();
                auto dim = p->getDimensionId();
                for (const auto &act : actions) {
                    auto cmd = fmt::format(act, fmt::arg("px", pos.x), fmt::arg("py", pos.y),
                                           fmt::arg("pz", pos.z), fmt::arg("pdim", dim));
                    p->runcmd(cmd);
                }
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

    //    管理命令
    void setup_shortcutCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = CREATE_CMD(shortcut, level);

        auto &listOpt = command->setEnum("list", {"list"});
        auto &modifyOpt = command->setEnum("modify", {"enable", "disable"});

        command->mandatory("shortcut", ParamType::Enum, listOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("shortcut", ParamType::Enum, modifyOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("name", ParamType::SoftEnum, command->setSoftEnum("name", {}));

        command->addOverload({listOpt});
        command->addOverload({modifyOpt, "name"});

        //        auto &modifyOpt = command->setEnum("modify", {"sub", "unsub"});

        //        auto &listOpt = command->setEnum("show", {"list"});
        //
        //        command->mandatory("trigger", ParamType::Enum, modifyOpt,
        //                           CommandParameterOption::EnumAutocompleteExpansion);
        //
        //        command->mandatory("trigger", ParamType::Enum, listOpt,
        //                           CommandParameterOption::EnumAutocompleteExpansion);
        //
        //        auto &showItemsOpt = command->setEnum("showItems",
        //        trapdoor::collectTrEventsStr());
        //
        //        command->mandatory("eventsType", ParamType::Enum, showItemsOpt,
        //                           CommandParameterOption::EnumAutocompleteExpansion);
        //
        //        command->addOverload({modifyOpt, "eventsType"});
        //        command->addOverload({listOpt});
        //

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            //            //  player needed
            //            if (!origin.getPlayer()) {
            //                ErrorPlayerNeed().sendTo(output);
            //                return;
            //            }
            //
            //            auto e = results["eventsType"].get<std::string>();
            //
            //            switch (do_hash(results["trigger"].getRaw<std::string>().c_str())) {
            //                case do_hash("sub"):
            //                    trapdoor::mod()
            //                        .getEventTriggerMgr()
            //                        .eventAction(origin.getName(), e, 0)
            //                        .sendTo(output);
            //                    break;
            //                case do_hash("unsub"):
            //                    trapdoor::mod()
            //                        .getEventTriggerMgr()
            //                        .eventAction(origin.getName(), e, 1)
            //                        .sendTo(output);
            //                    // TODO
            //                    break;
            //                case do_hash("list"):
            //                    trapdoor::mod()
            //                        .getEventTriggerMgr()
            //                        .listEvents(origin.getName())
            //                        .sendTo(output);
            //                    break;
            //            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor