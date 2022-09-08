//
// Created by xhy on 2022/5/17.
//

#include <MC/Abilities.hpp>
#include <MC/Ability.hpp>
#include <MC/BlockSource.hpp>
#include <MC/ChunkSource.hpp>
#include <MC/Dimension.hpp>
#include <MC/LayeredAbilities.hpp>
#include <MC/Level.hpp>
#include <MC/LevelChunk.hpp>
#include <string>

#include "CommandHelper.h"
#include "DynamicCommandAPI.h"
#include "MCTick.h"
#include "Particle.h"
#include "ScheduleAPI.h"
#include "SendPacketAPI.h"
namespace trapdoor {
    void print_ab(Player *p) {
        if (!p) return;
        auto &layeredAbilities = dAccess<LayeredAbilities, 2412>(p);
        layeredAbilities.forEachAbility(
            [](Ability const &ab, char const *name) {
                trapdoor::logger().debug("name: {} => {} {}", name, (int)ab.getType(), ab.isSet());
            },
            (Ability::Options)(0));
    }

    void test_ab(Player *p, int index, bool value) {
        if (!p) return;
        if (index == -1) {
            print_ab(p);
            return;
        }

        trapdoor::logger().debug("Set {} to  {}", index, value);
        auto &layeredAbilities = dAccess<LayeredAbilities, 2412>(p);
        auto &abs = dAccess<Abilities, 220>(&layeredAbilities);
        abs.unSet((AbilitiesIndex)index);
        // layeredAbilities.setAbility(static_cast<AbilitiesIndex>(index), value);
    }

    void setup_testCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("test", "change world tick command",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &optFreeze = command->setEnum("su", {"ability"});
        command->mandatory("test", ParamType::Enum, optFreeze,
                           CommandParameterOption::EnumAutocompleteExpansion);

        auto &optPrintAb = command->setEnum("pab", {"print"});
        command->mandatory("test", ParamType::Enum, optPrintAb,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->mandatory("ab", ParamType::Int);
        command->mandatory("onoroff", ParamType::Bool);
        command->addOverload({optFreeze, "ab", "onoroff"});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["test"].getRaw<std::string>().c_str())) {
                case do_hash("ability"):
                    test_ab(origin.getPlayer(), results["ab"].get<int>(),
                            results["onoroff"].get<bool>());
                default:
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor