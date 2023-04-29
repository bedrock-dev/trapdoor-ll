//
// Created by xhy on 2022/5/17.
//
#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "MCTick.h"
#include "Msg.h"
#include "SimpleProfiler.h"
#include "TrapdoorMod.h"

namespace trapdoor {
    void setup_profCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = CREATE_CMD(prof, level);
        auto &optContinue = command->setEnum("opt", {"normal", "chunk", "pt", "entity", "mspt"});
        command->mandatory("prof", ParamType::Enum, optContinue,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->optional("ticks", ParamType::Int);
        command->addOverload({optContinue, "ticks"});
        command->addOverload(std::vector<std::string>());

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            if (!results["prof"].isSet) {
                trapdoor::logger().debug("you run prof");
                trapdoor::startProfiler(20, SimpleProfiler::Normal).sendTo(output);
                return;
            }

            auto tickTime = results["ticks"].isSet ? results["ticks"].getRaw<int>() : 20;
            switch (do_hash(results["prof"].getRaw<std::string>().c_str())) {
                case do_hash("normal"):
                    trapdoor::startProfiler(tickTime, SimpleProfiler::Normal).sendTo(output);
                    break;
                case do_hash("chunk"):
                    trapdoor::startProfiler(tickTime, SimpleProfiler::Chunk).sendTo(output);
                    break;
                case do_hash("entity"):
                    trapdoor::startProfiler(tickTime, SimpleProfiler::Entity).sendTo(output);
                    break;
                case do_hash("pt"):
                    trapdoor::startProfiler(tickTime, SimpleProfiler::PendingTick).sendTo(output);
                case do_hash("mspt"):
                    trapdoor::startProfiler(tickTime, SimpleProfiler::Mspt).sendTo(output);
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor