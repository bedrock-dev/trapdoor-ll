//
// Created by xhy on 2022/5/17.
//
#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "SpawnHelper.h"
#include "TrapdoorMod.h"
namespace tr {
    void setup_funcCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("func", "trapdoor functions",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &hoppercounterOpt = command->setEnum("hoppercounter", {"hoppercounter"});
        command->mandatory("func", ParamType::Enum, hoppercounterOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        auto &blockrotateOpt = command->setEnum("blockrotate", {"blockrotate"});
        command->mandatory("func", ParamType::Enum, blockrotateOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        auto &hudOpt = command->setEnum("hud", {"hud"});
        command->mandatory("func", ParamType::Enum, hudOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->optional("onoroff", ParamType::Bool);
        command->addOverload({hoppercounterOpt, "onoroff"});
        command->addOverload({blockrotateOpt, "onoroff"});
        command->addOverload({hudOpt, "onoroff"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            auto countParam = std::string();
            switch (do_hash(results["func"].getRaw<std::string>().c_str())) {
                case do_hash("hoppercounter"):
                    tr::mod()
                        .getHopperChannelManager()
                        .setAble(results["onoroff"].get<bool>())
                        .sendTo(output);
                    break;
                case do_hash("hud"):
                    tr::mod().getHUDHelper().setAble(results["onoroff"].get<bool>()).sendTo(output);
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr
