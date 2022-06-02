//
// Created by xhy on 2022/5/17.
//
#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "MCTick.h"
#include "Msg.h"
#include "SimpleProfiler.h"

namespace tr {
    void setup_profCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand(
            "prof", "profile world health",
            static_cast<CommandPermissionLevel>(level));

        auto &optContinue =
            command->setEnum("opt", {"normal", "chunk", "pt", "entity"});
        command->mandatory("prof", ParamType::Enum, optContinue,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->optional("numberOfTick", ParamType::Int);
        command->addOverload({optContinue, "numberOfTick"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            auto tickTime = results["numberOfTick"].isSet
                                ? results["numberOfTick"].getRaw<int>()
                                : 20;
            switch (do_hash(results["prof"].getRaw<std::string>().c_str())) {
                case do_hash("normal"):
                    tr::StartProfiler(tickTime, SimpleProfiler::Normal)
                        .SendTo(output);
                case do_hash("chunk"):
                    tr::StartProfiler(tickTime, SimpleProfiler::Chunk)
                        .SendTo(output);
                    break;
                case do_hash("entity"):
                    tr::StartProfiler(tickTime, SimpleProfiler::Entity)
                        .SendTo(output);
                    break;
                case do_hash("pt"):
                    ErrorMsg("Function is developing by developer")
                        .SendTo(output);
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace tr