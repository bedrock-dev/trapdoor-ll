//
// Created by xhy on 2022/5/17.
//

#include <MC/Abilities.hpp>
#include <MC/Ability.hpp>
#include <MC/Block.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Brightness.hpp>
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
#include "TVec3.h"

namespace trapdoor {

    void show_light(Player *player) {
        if (!player) return;
        auto *bs = player->getBlockSource();

        auto pos = player->getPos().toBlockPos();

        for (int i = -7; i < 7; i++) {
            for (int j = -2; j < 3; j++) {
                for (int k = -7; k < 7; k++) {
                    // TODO
                    auto p = pos + BlockPos(i, j, k);
                    auto &b1 = bs->getBlock(p);
                    auto &b2 = bs->getBlock(p + BlockPos(0, -1, 0));
                    if (b1.isAir() && (!b2.isAir())) {
                        auto rb = bs->getRawBrightness(pos + BlockPos(i, j, k), true, true);
                        auto bright = (uint32_t) * reinterpret_cast<unsigned char *>(&rb);

                        auto color = trapdoor::PCOLOR::GREEN;
                        if (bright < 8) color = trapdoor::PCOLOR::YELLOW;
                        if (bright < 1) color = trapdoor::PCOLOR::RED;
                        trapdoor::spawnNumParticle(
                            trapdoor::TVec3(p.x + 0.5f, p.y + 0.2f, p.z + 0.5f), (int)bright, color,
                            0);
                    }
                }
            }
        }

        // auto rb = bs.getRawBrightness(pos + BlockPos(0, 1, 0), true, true);
        // auto bright = (uint32_t) * reinterpret_cast<unsigned char *>(&rb);

        //
    }
    void setup_testCommand(int level) {
        using ParamType = DynamicCommand::ParameterType;
        // create a dynamic command
        auto command = DynamicCommand::createCommand("test", "change world tick command",
                                                     static_cast<CommandPermissionLevel>(level));

        auto &optPrintAb = command->setEnum("pab", {"print"});
        command->mandatory("test", ParamType::Enum, optPrintAb,
                           CommandParameterOption::EnumAutocompleteExpansion);

        command->addOverload({optPrintAb});

        auto cb = [](DynamicCommand const &command, CommandOrigin const &origin,
                     CommandOutput &output,
                     std::unordered_map<std::string, DynamicCommand::Result> &results) {
            switch (do_hash(results["test"].getRaw<std::string>().c_str())) {
                case do_hash("print"):
                    show_light(origin.getPlayer());
                    break;
            }
        };
        command->setCallback(cb);
        DynamicCommand::setup(std::move(command));
    }

}  // namespace trapdoor