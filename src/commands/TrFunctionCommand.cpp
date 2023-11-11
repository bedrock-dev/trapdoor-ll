//
// Created by xhy on 2022/5/17.
//
#include "BlockRotateHelper.h"
#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "SpawnHelper.h"
#include "TrapdoorMod.h"

#define ADD_CONFIG(name, hint)                                             \
    auto &name##Opt = command->setEnum(#name, {#name});                    \
    command->mandatory("func", ParamType::Enum, name##Opt,                 \
                       CommandParameterOption::EnumAutocompleteExpansion); \
    command->addOverload({name##Opt, #hint});

#define ADD_INT_CONFIG(name) ADD_CONFIG(name, value)
#define ADD_BOOL_CONFIG(name) ADD_CONFIG(name, onoroff)

namespace trapdoor {

    template <typename T>
    void globalFunctionChangeListener(const std::string &key, T value) {
        trapdoor::broadcastMessage(trapdoor::SetValueMsg(key, value).msg, -1);
        if (key == "noclip") {
            Global<Level>->forEachPlayer([value](Player &p) {
                if (!value) {  // 全局指令关闭，直接强制全部关闭
                    p.setAbility(static_cast<AbilitiesIndex>(17), false);
                } else {  // 全局开启且自己开启 且是创造模式就自动更新 这里获取不到
                          // 不写了，需要玩家手动刷新
                    if (p.getPlayerGameType() == GameType::Creative) {
                        // TODO
                    }
                };
                return true;
            });
        } else if (key == "hoppercounter") {
            trapdoor::logger().debug("Set {} to {}", key, value);
            if (!value) {
                trapdoor::mod().getHopperChannelManager().clearAllData();
            }
        }
        // other options
    }

    const DynamicCommandInstance *setup_funcCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        auto command = CREATE_CMD(func, level);

        command->optional("onoroff", ParamType::Bool);
        command->optional("value", ParamType::Int);

        ADD_BOOL_CONFIG(blockrotate)
        ADD_BOOL_CONFIG(hud)
        ADD_BOOL_CONFIG(hoppercounter)

        ADD_INT_CONFIG(fcplace)
        ADD_BOOL_CONFIG(fcopen)
        ADD_BOOL_CONFIG(nocost)
        ADD_BOOL_CONFIG(autotool)
        ADD_INT_CONFIG(maxptsize)
        ADD_BOOL_CONFIG(safeexplode)
        //        ADD_BOOL_CONFIG(noncupdate)
        ADD_BOOL_CONFIG(noclip)

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            auto iValue = results["value"];
            auto bValue = results["onoroff"];

            auto i = iValue.isSet ? iValue.get<int>() : -1;  //-1 is invalid value
            auto b = bValue.isSet && bValue.get<bool>();

            switch (do_hash(results["func"].getRaw<std::string>().c_str())) {
#define ADD_BOOL_CASE(cmd, var)                                                                    \
    case do_hash(#cmd):                                                                            \
        if (bValue.isSet) {                                                                        \
            trapdoor::mod().getConfig().getGlobalFunctionConfig().var = b;                         \
            globalFunctionChangeListener(#cmd, b);                                                 \
            trapdoor::SetValueMsg(#var, b).sendTo(output);                                         \
        } else {                                                                                   \
            trapdoor::GetValueMsg(#var, trapdoor::mod().getConfig().getGlobalFunctionConfig().var) \
                .sendTo(output);                                                                   \
        }                                                                                          \
        break;

#define ADD_INT_CASE(cmd, var, min, max)                                                           \
    case do_hash(#cmd):                                                                            \
        if (iValue.isSet) {                                                                        \
            if (i >= (min) && i <= (max)) {                                                        \
                trapdoor::mod().getConfig().getGlobalFunctionConfig().var = i;                     \
                trapdoor::SetValueMsg(#var, i).sendTo(output);                                     \
            } else {                                                                               \
                trapdoor::ErrorRange(#var, min, max).sendTo(output);                               \
            }                                                                                      \
        } else {                                                                                   \
            trapdoor::GetValueMsg(#var, trapdoor::mod().getConfig().getGlobalFunctionConfig().var) \
                .sendTo(output);                                                                   \
        }                                                                                          \
        break;
                ADD_BOOL_CASE(blockrotate, blockRotate)
                ADD_BOOL_CASE(hud, hud)
                ADD_BOOL_CASE(hoppercounter, hopperCounter)
                ADD_BOOL_CASE(fcopen, forceOpenContainer)
                ADD_BOOL_CASE(nocost, dropperNoCost)
                ADD_BOOL_CASE(autotool, autoSelectTool)
                ADD_BOOL_CASE(safeexplode, safeExplosion)
                ADD_BOOL_CASE(noclip, creativeNoClip)
                ADD_INT_CASE(fcplace, forcePlaceLevel, 0, 2)
                ADD_INT_CASE(maxptsize, maxPendingTickSize, 0, 0xffffff);
                default:
                    break;
            }
        };
        command->setCallback(cb);
        return DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor
