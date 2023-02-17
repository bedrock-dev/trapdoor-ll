//
// Created by xhy on 2022/5/17.
//
#include "BlockRotateHelper.h"
#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "SpawnHelper.h"
#include "TrapdoorMod.h"


#define  ADD_CONFIG(name, hint)       auto &name##Opt = command->setEnum(#name, {#name}); \
        command->mandatory("func", ParamType::Enum, name##Opt,CommandParameterOption::EnumAutocompleteExpansion); \
        command->addOverload({name##Opt, #hint});


#define  ADD_INT_CONFIG(name) ADD_CONFIG(name,value)
#define  ADD_BOOL_CONFIG(name) ADD_CONFIG(name,onoroff)

namespace trapdoor {
    void setup_funcCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        auto command = CREATE_CMD(func, level);

        command->optional("onoroff", ParamType::Bool);
        command->optional("value", ParamType::Int);

        ADD_BOOL_CONFIG(blockrotate)
        ADD_BOOL_CONFIG(hud)
        ADD_BOOL_CONFIG(hoppercountert)

        ADD_INT_CONFIG(fcplace)
        ADD_BOOL_CONFIG(fcopen)
        ADD_BOOL_CONFIG(nocost)
        ADD_BOOL_CONFIG(autotool)
        ADD_INT_CONFIG(maxptsize)
        ADD_BOOL_CONFIG(safeexplode)
        ADD_BOOL_CONFIG(noncupdate)
        ADD_BOOL_CONFIG(noclip)


        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            auto countParam = std::string();
            switch (do_hash(results["func"].getRaw<std::string>().c_str())) {

//                case do_hash("hoppercounter"):
//                    trapdoor::mod()
//                            .getHopperChannelManager()
//                            .setAble(results["onoroff"].get<bool>())
//                            .sendTo(output);
//                    break;
//                case do_hash("hud"):
//                    trapdoor::mod()
//                            .getHUDHelper()
//                            .setAble(results["onoroff"].get<bool>())
//                            .sendTo(output);
//                    break;
//                case do_hash("blockrotate"):
//                    trapdoor::setBlockRotationAble(results["onoroff"].get<bool>()).sendTo(output);
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor
