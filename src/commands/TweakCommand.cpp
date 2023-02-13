//
// Created by xhy on 2022/6/29.
//
#include "CommandHelper.h"
#include "Configuration.h"
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
        return trapdoor::OperationSuccess();
    }

    ActionResult setForceOpenContainer(bool able) {
        trapdoor::mod().getConfig().getTweakConfig().forceOpenContainer = able;
        return trapdoor::OperationSuccess();
    }
    ActionResult setDropperNoCost(bool able) {
        trapdoor::mod().getConfig().getTweakConfig().dropperNoCost = able;
        return trapdoor::OperationSuccess();
    }
    ActionResult setAutoTools(bool able) {
        trapdoor::mod().getConfig().getTweakConfig().autoSelectTool = able;
        return trapdoor::OperationSuccess();
    }
    ActionResult setSafeExplosion(bool able) {
        trapdoor::mod().getConfig().getTweakConfig().safeExplosion = able;
        return trapdoor::OperationSuccess();
    }
    ActionResult setDisableNCUpdate(bool able) {
        trapdoor::mod().getConfig().getTweakConfig().disableNCUpdate = able;
        return trapdoor::OperationSuccess();
    }
    ActionResult setCreativeNoClip(bool able) {
        trapdoor::mod().getConfig().getTweakConfig().creativeNoClip = able;
        Global<Level>->forEachPlayer([able](Player &p) {
            if (p.getPlayerGameType() == GameType::GameTypeCreative) {
                p.setAbility(static_cast<AbilitiesIndex>(17), able);
            }
            return true;
        });
        return trapdoor::OperationSuccess();
    }

    void setup_tweakCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        auto command = CREATE_CMD(tweak, level);
        auto &forcePlaceOpt = command->setEnum("place", {"fcplace"});
        auto &forceOpenContainer = command->setEnum("open", {"fcopen"});
        auto &dropperNoCost = command->setEnum("cost", {"nocost"});
        auto &autoSelectTools = command->setEnum("autoSelectTool", {"autotool"});
        auto &maxPendingTickSize = command->setEnum("maxPendingTickSize", {"maxptsize"});
        auto &safeExplosion = command->setEnum("safeExplosion", {"safeexplode"});
        auto &disableNUpdate = command->setEnum("disableNCUpdate", {"noncupdate"});
        auto &creativeNoClip = command->setEnum("creativeNoClip", {"noclip"});

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
        command->mandatory("tweak", ParamType::Enum, creativeNoClip,
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
        command->addOverload({creativeNoClip, "onoroff"});

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
                case do_hash("noclip"):
                    setCreativeNoClip(results["onoroff"].get<bool>()).sendTo(output);
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor