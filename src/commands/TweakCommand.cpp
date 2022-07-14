//
// Created by xhy on 2022/6/29.
//
#include "CommandHelper.h"
#include "Config.h"
#include "DynamicCommandAPI.h"
#include "TrapdoorMod.h"
namespace trapdoor {
    ActionResult setForcePlaceBlock(int level) {
        if (level < 0 || level > 2) {
            return {"Level should within [0,2]", false};
        }
        trapdoor::mod().getConfig().getTweakConfig().forcePlaceLevel = level;
        return {"Success", true};
    }
    ActionResult setForceOpenContainer(bool able) {
        trapdoor::mod().getConfig().getTweakConfig().forceOpenContainer = able;
        return {"Success", true};
    }
    ActionResult setDropperNoCost(bool able) {
        trapdoor::mod().getConfig().getTweakConfig().dropperNoCost = able;
        return {"Success", true};
    }
    ActionResult setAutoTools(bool able) {
        trapdoor::mod().getConfig().getTweakConfig().autoSelectTool = able;
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
        auto &dropperNoCost = command->setEnum("cost", {"nocost"});
        auto &autoSelectTools = command->setEnum("autoSelecttool", {"autotool"});

        command->mandatory("tweak", ParamType::Enum, forcePlaceOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tweak", ParamType::Enum, forceOpenContainer,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("tweak", ParamType::Enum, dropperNoCost,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("tweak", ParamType::Enum, autoSelectTools,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("level", ParamType::Int);
        command->mandatory("onoroff", ParamType::Bool);

        command->addOverload({forcePlaceOpt, "level"});
        command->addOverload({forceOpenContainer, "onoroff"});
        command->addOverload({dropperNoCost, "onoroff"});
        command->addOverload({autoSelectTools, "onoroff"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["tweak"].getRaw<std::string>().c_str())) {
                case do_hash("fcplace"):
                    setForcePlaceBlock(results["level"].get<int>()).sendTo(output);
                    break;
                case do_hash("fcopen"):
                    setForceOpenContainer(results["onoroff"].get<bool>()).sendTo(output);
                    break;
                case do_hash("nocost"):
                    setDropperNoCost(results["onoroff"].get<bool>()).sendTo(output);
                    break;
                case do_hash("autotool"):
                    setAutoTools(results["onoroff"].get<bool>()).sendTo(output);
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor