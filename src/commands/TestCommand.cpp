//
// Created by xhy on 2022/5/17.
//

#include <mc/Abilities.hpp>
#include <mc/Ability.hpp>
#include <mc/Block.hpp>
#include <mc/BlockSource.hpp>
#include <mc/Brightness.hpp>
#include <mc/ChunkSource.hpp>
#include <mc/Dimension.hpp>
#include <mc/LayeredAbilities.hpp>
#include <mc/Level.hpp>
#include <mc/LevelChunk.hpp>
#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "MCTick.h"
#include "Particle.h"
#include "ScheduleAPI.h"
#include "SendPacketAPI.h"
#include "TVec3.h"

namespace trapdoor {

    void setup_testCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("test", "Command for developing",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &optOpenHome = command->setEnum("openHome", {"home"});
        command->mandatory("test", ParamType::Enum, optOpenHome,
                           CommandParameterOption::EnumAutocompleteExpansion);
        command->addOverload({optOpenHome});
        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["test"].getRaw<std::string>().c_str())) {
                case do_hash("home"):
                    system("explorer.exe .");
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor