//
// Created by xhy on 2022/5/17.
//

#include <MC/BlockSource.hpp>
#include <MC/ChunkSource.hpp>
#include <MC/Dimension.hpp>
#include <MC/Level.hpp>
#include <MC/LevelChunk.hpp>
#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "MCTick.h"
#include "Particle.h"
#include "ScheduleAPI.h"
namespace trapdoor {
    void test_particle(Player * p) {
        //        Schedule::repeat(
        //            []() {
        //                trapdoor::drawLine({0, 80, 0}, TFACING::POS_X, 120, PCOLOR::GREEN, 0);
        //            },
        //            20);
    }

    void setup_testCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("test", "change world tick command",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &optFreeze = command->setEnum("su", {"particle"});
        command->mandatory("test", ParamType::Enum, optFreeze,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optFreeze});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["test"].getRaw<std::string>().c_str())) {
                case do_hash("particle"):
                    test_particle(origin.getPlayer());
                    break;
                default:
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor