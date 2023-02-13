//
// Created by xhy on 2022/10/5.
//
#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "HookAPI.h"
#include "MCTick.h"
#include "Msg.h"
#include "RandomTweaker.h"
#include "SimpleProfiler.h"
#include "TrapdoorMod.h"
namespace trapdoor {

    void setup_randCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = CREATE_CMD(rand, level);

        auto &optType = command->setEnum("typeCmd", {"normal", "gauss"});

        auto &resetSubOpt = command->setEnum("resetOpt", {"reset"});
        auto &rangeSubOpt = command->setEnum("rangeOpt", {"range"});
        auto &fixSubOpt = command->setEnum("fixOpt", {"fix"});
        auto &scaleSubOpt = command->setEnum("scaleOpt", {"scale"});

        command->mandatory("rand", ParamType::Enum, optType,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("resetOpt", ParamType::Enum, resetSubOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("rangeOpt", ParamType::Enum, rangeSubOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("fixOpt", ParamType::Enum, fixSubOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->mandatory("scaleOpt", ParamType::Enum, scaleSubOpt,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("value", ParamType::Int);
        command->mandatory("min", ParamType::Int);
        command->mandatory("max", ParamType::Int);
        command->mandatory("scale", ParamType::Float);

        command->addOverload({optType, "fixOpt", "value"});
        command->addOverload({optType, "resetOpt"});
        command->addOverload({optType, "rangeOpt", "min", "max"});
        command->addOverload({optType, "scaleOpt", "scale"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            int fixed_value = results["value"].isSet ? results["value"].get<int>() : -1;
            int min_value = results["min"].isSet ? results["min"].get<int>() : -1;
            int max_value = results["max"].isSet ? results["max"].get<int>() : -1;
            int scale_value = results["scale"].isSet ? results["scale"].get<int>() : -1;
            bool fixOpt = results["fixOpt"].isSet;
            bool resetOpt = results["resetOpt"].isSet;
            bool rangeOpt = results["rangeOpt"].isSet;
            bool scaleOpt = results["scaleOpt"].isSet;
            auto type =
                results["rand"].isSet ? results["rand"].getRaw<std::string>().c_str() : "unknown";

            trapdoor::logger().debug(
                "type: {} action: {}|{}|{}|{} value: {} min:{} max:{} scale:{}", type, fixOpt,
                resetOpt, rangeOpt, scaleOpt, fixed_value, min_value, max_value, scale_value);
            if (fixOpt) {
                trapdoor::tweakFixedRandomNumber(type, fixed_value).sendTo(output);
                return;
            }
            // TODO 另外三个情况
        };

        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor
