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
    void registerShortcutCommand(const std::string &shortcut,
                                 const std::vector<std::string> &actions) {
        tr::logger().debug("registerShortcutCommand: {} => ", shortcut);
        for (auto &action : actions) {
            tr::logger().debug("        - {}", action);
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
                for (const auto &act : actions) {
                    auto cmd = fmt::format(act, fmt::arg("px", pos.x), fmt::arg("py", pos.y),
                                           fmt::arg("pz", pos.z));
                    p->runcmd(cmd);
                }
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr