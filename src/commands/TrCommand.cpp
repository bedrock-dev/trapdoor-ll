#include "DynamicCommandAPI.h"
#include <unordered_map>

void SetupTickCommand() {

    using ParamType = DynamicCommand::ParameterType;
    // create a dynamic command
    auto command = DynamicCommand::createCommand("tick", "change world tick command",
                                                 CommandPermissionLevel::GameMasters);


    auto &optForward = command->setEnum("forward", {"forward"});
    command->mandatory("tick", ParamType::Enum, optForward, CommandParameterOption::EnumAutocompleteExpansion);
    command->mandatory("tickNumber", ParamType::Int);
    command->addOverload({optForward, "tickNumber"});

    auto &optSpeedChange = command->setEnum("speedChange", {"accelerate", "slow"});
    command->mandatory("tick", ParamType::Enum, optSpeedChange, CommandParameterOption::EnumAutocompleteExpansion);
    command->mandatory("times", ParamType::Int);
    command->addOverload({optSpeedChange, "times"});

    auto &optFreeze = command->setEnum("freeze", {"freeze", "reset", "query"});
    command->mandatory("tick", ParamType::Enum, optFreeze, CommandParameterOption::EnumAutocompleteExpansion);
    command->addOverload({optFreeze});

    auto cb = [](DynamicCommand const &command, CommandOrigin const &origin, CommandOutput &output,
                 std::unordered_map<std::string, DynamicCommand::Result> &results) {
        switch (do_hash(results["tick"].getRaw<std::string>().c_str())) {
            case do_hash("accelerate"):
                output.success(fmt::format("Acc {} times", results["times"].getRaw<int>()));
                break;
            case do_hash("slow"):
                output.success(fmt::format("slow {} times", results["times"].getRaw<int>()));
                break;
            case do_hash("forward"):
                output.success(fmt::format("Remove - {}", results["tickNumber"].getRaw<int>()));
                break;
            case do_hash("freeze"):
                output.success("success");
            default:
                break;
        }
    };

    command->setCallback(cb);
    DynamicCommand::setup(std::move(command));
}

void SetupProfCommand() {
    using ParamType = DynamicCommand::ParameterType;
    // create a dynamic command
    auto command = DynamicCommand::createCommand("prof", "profile world health",
                                                 CommandPermissionLevel::GameMasters);

    auto &optContinue = command->setEnum("continue", {"continue"});
    command->mandatory("prof", ParamType::Enum, optContinue, CommandParameterOption::EnumAutocompleteExpansion);
    command->mandatory("NumberOfTick", ParamType::Int);
    command->addOverload({optContinue, "NumberOfTick"});

    auto &optOther = command->setEnum("other", {"chunk", "pt", "actor"});
    command->mandatory("prof", ParamType::Enum, optOther, CommandParameterOption::EnumAutocompleteExpansion);
    command->addOverload({optOther});

    auto cb = [](DynamicCommand const &command, CommandOrigin const &origin, CommandOutput &output,
                 std::unordered_map<std::string, DynamicCommand::Result> &results) {};
    command->setCallback(cb);

    DynamicCommand::setup(std::move(command));
}


void SetupVillageCommand() {
    using ParamType = DynamicCommand::ParameterType;
    // create a dynamic command
    auto command = DynamicCommand::createCommand("village", "village helper",
                                                 CommandPermissionLevel::GameMasters);

    auto &optSwitch = command->setEnum("switch", {"bound", "spawn", "center", "poi", "head"});
    command->mandatory("village", ParamType::Enum, optSwitch, CommandParameterOption::EnumAutocompleteExpansion);
    command->mandatory("onOroff", ParamType::Bool);
    command->addOverload({optSwitch, "onOroff"});

    auto &optOther = command->setEnum("other", {"list", "near"});
    command->mandatory("village", ParamType::Enum, optOther, CommandParameterOption::EnumAutocompleteExpansion);
    command->addOverload({optOther});
    auto cb = [](DynamicCommand const &command, CommandOrigin const &origin, CommandOutput &output,
                 std::unordered_map<std::string, DynamicCommand::Result> &results) {};
    command->setCallback(cb);
    DynamicCommand::setup(std::move(command));
}
