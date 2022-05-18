//
// Created by xhy on 2022/5/17.
//
#include "DynamicCommandAPI.h"
#include <unordered_map>
#include "MCTick.h"

namespace tr {

    void SetupLogCommand() {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("log", "print some world info",
                                                     CommandPermissionLevel::GameMasters);

        auto &optMain = command->setEnum("main", {"mspt", "tps"});
        command->mandatory("log", ParamType::Enum, optMain, CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optMain});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin, CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["log"].getRaw<std::string>().c_str())) {
                case do_hash("mspt"):
                    output.success(fmt::format("MSPT is {} ms", tr::getMeanMSPT()));
                    break;
                case do_hash("tps"):
                    output.success(fmt::format("TPS is {} ms", tr::getMeanTPS()));
                    break;
                default:
                    break;
            }
        };
        command->setCallback(cb);

        DynamicCommand::setup(std::move(command));
    }
}