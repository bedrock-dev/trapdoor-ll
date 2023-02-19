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

#define  SET_FUNC_NAME(item) set_##item
#define  GET_FUNC_NAME(item) get_##item

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

        auto &dumpOpt = command->setEnum("dump", {"dump"}); \
        command->mandatory("self", ParamType::Enum, dumpOpt, CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({dumpOpt});

        // ADD_BOOL_CONFIG(fcopen)
        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {

            if (!origin.getPlayer()) {
                trapdoor::ErrorPlayerNeed().sendTo(output);
                return;
            }
            auto iValue = results["value"];
            auto bValue = results["onoroff"];

            [[maybe_unused]] auto i = iValue.isSet ? iValue.get<int>() : -1; //-1 is invalid value
            auto b = bValue.isSet && bValue.get<bool>();

            auto playerName = origin.getPlayer()->getRealName();
            switch (do_hash(results["self"].getRaw<std::string>().c_str())) {

#define ADD_BOOL_CASE(name)   case do_hash(#name): if (bValue.isSet) {trapdoor::mod(). \
                        getUserConfig().SET_FUNC_NAME(name)(playerName, b).sendTo(output);\
                        } else {trapdoor::mod().getUserConfig().GET_FUNC_NAME(name)(playerName).sendTo(output);}break;
                //TODO 等有整数型再设置
#define ADD_INT_CASE
                ADD_BOOL_CASE(hud)
                ADD_BOOL_CASE(noclip);
                ADD_BOOL_CASE(blockrotate);
                ADD_BOOL_CASE(autotool);
                //          ADD_BOOL_CASE(fcopen);
                case do_hash("dump"):
                    trapdoor::mod().getUserConfig().dumpAllSelfConfig(playerName).sendTo(output);
                default:
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }
}  // namespace trapdoor

