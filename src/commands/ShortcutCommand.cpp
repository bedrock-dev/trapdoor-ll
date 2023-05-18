//
// Created by xhy on 2022/5/17.
//
#include <mc/Player.hpp>
#include <mc/ServerPlayer.hpp>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"
#include "config.h"
namespace trapdoor {

    ActionResult getShortcutsInfo() {
        TextBuilder builder;
        auto &scs = trapdoor::mod().getConfig().getShortcuts();
        for (auto &sh : scs) {
            auto color = sh.second.enable ? TextBuilder::GREEN : TextBuilder::RED;
            builder.sText(TB::GRAY, " - ")
                .sTextF(color, "[%s] ", sh.first.c_str())
                .textF("%s\n", sh.second.getDescription().c_str());
        }
        return SuccessMsg(builder.get());
    }

    std::vector<std::string> getShortcutNameList() {
        std::vector<std::string> res;
        auto &scs = trapdoor::mod().getConfig().getShortcuts();
        for (auto &sh : scs) {
            res.push_back(sh.first);
        }
        return res;
    }

    ActionResult modifyShortcuts(const std::string &name, bool value) {
        auto &scs = trapdoor::mod().getConfig().getShortcuts();
        auto it = scs.find(name);
        if (it == scs.end()) {
            return ErrorMsg(trapdoor::format("Unknown shortcut: %s", name.c_str()));
        }

        it->second.enable = value;
        return trapdoor::SetValueMsg(name, value);
    }

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
    const DynamicCommandInstance * setup_shortcutCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = CREATE_CMD(shortcut, level);

        auto &listOpt = command->setEnum("list", {"list"});
        auto &modifyOpt = command->setEnum("modify", {"enable", "disable"});

        command->mandatory("shortcut", ParamType::Enum, listOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("shortcut", ParamType::Enum, modifyOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("name", ParamType::SoftEnum,
                           command->setSoftEnum("name", getShortcutNameList()));

        command->addOverload({listOpt});
        command->addOverload({modifyOpt, "name"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            auto name = results["name"].isSet ? results["name"].get<std::string>() : std::string();
            switch (do_hash(results["shortcut"].getRaw<std::string>().c_str())) {
                case do_hash("list"):
                    getShortcutsInfo().sendTo(output);
                    break;

                case do_hash("enable"):
                    modifyShortcuts(name, true).sendTo(output);
                    break;
                case do_hash("disable"):
                    modifyShortcuts(name, false).sendTo(output);
                    break;
            }
        };
        command->setCallback(cb);
      return  DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor