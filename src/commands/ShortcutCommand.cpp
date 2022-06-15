//
// Created by xhy on 2022/5/17.
//
#include <MC/Player.hpp>
#include <MC/ServerPlayer.hpp>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"
namespace tr {
    void registerShortcutCommand(const std::string &shortcut, const std::string &action) {
        tr::logger().debug("registerShortcutCommand: {} {}", shortcut, action);
        using ParamType = DynamicCommand::ParameterType;
        auto description = "Shortcut for /" + action;
        auto command = DynamicCommand::createCommand(shortcut, description);
        command->addOverload(std::vector<std::string>{});
        auto cb = [action](DynamicCommand const &command, CommandOrigin const &origin,
                           CommandOutput &output,
                           std::unordered_map<std::string, DynamicCommand::Result> &results) {
            auto *p = origin.getPlayer();
            if (p) {
                p->runcmd(action);
            }

        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr