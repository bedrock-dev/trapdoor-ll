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
    ActionResult setMaxPendingTickSize(int size) {
        if (size < 0 || size > 0xffffff) {
            return {"Size should within [0,0xffffff]", false};
        }

        trapdoor::mod().getConfig().getTweakConfig().maxPendingTickSize = size;
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
    ActionResult setSafeExplosion(bool able) {
        trapdoor::mod().getConfig().getTweakConfig().safeExplosion = able;
        return {"Success", true};
    }
    ActionResult setDisableNCUpdate(bool able) {
        trapdoor::mod().getConfig().getTweakConfig().disableNCUpdate = able;
        return {"Success", true};
    }

    void setup_tweakCommand(int level) {
        auto command = DynamicCommand::createCommand("tweak", "Tweak vanilla features",
                                                     static_cast<CommandPermissionLevel>(level));
        using ParamType = DynamicCommand::ParameterType;

        auto &forcePlaceOpt = command->setEnum("place", {"fcplace"});
        auto &forceOpenContainer = command->setEnum("open", {"fcopen"});
        auto &dropperNoCost = command->setEnum("cost", {"nocost"});
        auto &autoSelectTools = command->setEnum("autoSelectTool", {"autotool"});
        auto &maxPendingTickSize = command->setEnum("maxPendingTickSize", {"maxptsize"});
        auto &safeExplosion = command->setEnum("safeExplosion", {"safeexplode"});
        auto &disableNUpdate = command->setEnum("disableNCUpdate", {"noncupdate"});

        command->mandatory("tweak", ParamType::Enum, forcePlaceOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tweak", ParamType::Enum, forceOpenContainer,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tweak", ParamType::Enum, dropperNoCost,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tweak", ParamType::Enum, autoSelectTools,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tweak", ParamType::Enum, maxPendingTickSize,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tweak", ParamType::Enum, safeExplosion,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("tweak", ParamType::Enum, disableNUpdate,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("value", ParamType::Int);
        command->mandatory("onoroff", ParamType::Bool);

        command->addOverload({forcePlaceOpt, "value"});
        command->addOverload({forceOpenContainer, "onoroff"});
        command->addOverload({dropperNoCost, "onoroff"});
        command->addOverload({autoSelectTools, "onoroff"});
        command->addOverload({safeExplosion, "onoroff"});
        command->addOverload({disableNUpdate, "onoroff"});
        command->addOverload({maxPendingTickSize, "value"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["tweak"].getRaw<std::string>().c_str())) {
                case do_hash("fcplace"):
                    setForcePlaceBlock(results["value"].get<int>()).sendTo(output);
                    break;
                case do_hash("maxptsize"):
                    setMaxPendingTickSize(results["value"].get<int>()).sendTo(output);
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
                case do_hash("safeexplode"):
                    setSafeExplosion(results["onoroff"].get<bool>()).sendTo(output);
                    break;
                case do_hash("noncupdate"):
                    setDisableNCUpdate(results["onoroff"].get<bool>()).sendTo(output);
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor