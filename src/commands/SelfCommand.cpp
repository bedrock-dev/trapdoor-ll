//
// Created by xhy on 2023/2/17.
//
#include "BlockRotateHelper.h"
#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "SpawnHelper.h"
#include "TrapdoorMod.h"


#define  ADD_CONFIG(name, hint)       auto &name##Opt = command->setEnum(#name, {#name}); \
        command->mandatory("self", ParamType::Enum, name##Opt,CommandParameterOption::EnumAutocompleteExpansion); \
        command->addOverload({name##Opt, #hint});


#define  ADD_INT_CONFIG(name) ADD_CONFIG(name,value)
#define  ADD_BOOL_CONFIG(name) ADD_CONFIG(name,onoroff)

#define  FUNC_NAME(item) set_##item

//可只对个人生效的配置项目，默认全部开启(不需要自动关闭即可)
namespace trapdoor {
    void setup_selfCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        auto command = CREATE_CMD(self, level);
        command->optional("onoroff", ParamType::Bool);
        command->optional("value", ParamType::Int);
        ADD_BOOL_CONFIG(hud)
        ADD_BOOL_CONFIG(noclip)
        ADD_BOOL_CONFIG(blockrotate)
        ADD_BOOL_CONFIG(autotool)
        ADD_BOOL_CONFIG(fcopen)
        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            int value = results["value"].isSet ? results["value"].get<int>() : -1;
            bool onoroff = results["onoroff"].isSet ? results["value"].get<int>() : -1;

            if (!origin.getPlayer())return trapdoor::ErrorPlayerNeed();
            auto playerName = origin.getPlayer()->getRealName();
            switch (do_hash(results["self"].getRaw<std::string>().c_str())) {

#define ADD_CASE(name) case do_hash(#name): trapdoor::mod().getUserConfig(). \
FUNC_NAME(name)(playerName, onoroff).sendTo(output); break
                ADD_CASE(hud);
                ADD_CASE(noclip);
                ADD_CASE(blockrotate);
                ADD_CASE(autotool);
                ADD_CASE(fcopen);
            }

        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor

