//
// Created by xhy on 2022/6/29.
//
#include "CommandHelper.h"
#include "Config.h"
#include "DynamicCommandAPI.h"
#include "TrapdoorMod.h"
namespace tr {

    ActionResult setForcePlaceBlock(int level) {
        if (level < 0 || level > 2) {
            return {"Level should within [0,2]", false};
        }
        tr::mod().getConfig().getTweakConfig().forcePlaceLevel = level;
        return {"Success", true};
    }

    void setup_tweakCommand(int level) {
        // create a dynamic command
        auto command = DynamicCommand::createCommand("tweak", "Tweak vanilla features",
                                                     static_cast<CommandPermissionLevel>(level));
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command

        auto &forcePlaceOpt = command->setEnum("place", {"fcplace"});
        auto &forceOpenContainer = command->setEnum("open", {"fcopen"});
        command->mandatory("tweak", ParamType::Enum, forcePlaceOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tweak", ParamType::Enum, forceOpenContainer,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("level", ParamType::Int);
        command->mandatory("onoroff", ParamType::Bool);

        command->addOverload({forcePlaceOpt, "level"});
        command->addOverload({forceOpenContainer, "onoroff"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["tweak"].getRaw<std::string>().c_str())) {
                case do_hash("fcplace"):
                    setForcePlaceBlock(results["level"].get<int>());
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace tr