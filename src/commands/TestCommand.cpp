//
// Created by xhy on 2022/5/17.
//

#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "MCTick.h"
#include "Particle.h"
#include "ScheduleAPI.h"

namespace tr {

    void test_particle() {
        Schedule::repeat(
            []() {
                tr::DrawLine({0, 80, 0}, TFACING::POS_X, 120, PCOLOR::GREEN, 0);
            },
            20);
    }

    void SetupTestCommand() {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command =
            DynamicCommand::createCommand("test", "change world tick command",
                                          CommandPermissionLevel::GameMasters);

        auto &optFreeze = command->setEnum("su", {"particle"});
        command->mandatory("test", ParamType::Enum, optFreeze,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optFreeze});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result>
                         &results) {
            switch (do_hash(results["test"].getRaw<std::string>().c_str())) {
                case do_hash("particle"):
                    test_particle();
                    break;
                default:
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace tr