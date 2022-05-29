#include "SpawnHelper.h"

#include <MC/BlockSource.hpp>
#include <MC/Dimension.hpp>
#include <MC/Level.hpp>
#include <MC/LevelChunk.hpp>
#include <MC/Player.hpp>
#include <MC/Spawner.hpp>

#include "DataConverter.h"
#include "GlobalServiceAPI.h"

namespace tr {
    ActionResult PrintCap() {
        auto &sp = Global<Level>->getSpawner();

        return {"", true};
    }

    ActionResult CountActors(Player *p, const std::string &type) {
        //  auto chunkPos = tr::fromBlockPos(p->getBlockPos()).toChunkPos();
        //  auto chunk = p->getRegion().getChunk(chunkPos.x, chunkPos.z);
        // auto actors = chunk->getEntities();

        // for (auto &a : actors) {
        // }
        return {"", true};
    }

    ActionResult printSpawnProbability(Player *player, const BlockPos &pos) {
        auto &dim = player->getDimension();
        BlockPos topPos = {pos.x, 320, pos.z};
        bool isSurface = true;
        bool hasFound = false;
        while (topPos.y > -64) {
            Spawner::findNextSpawnBlockUnder(player->getRegion(), topPos,
                                             static_cast<MaterialType>(52),
                                             (SpawnBlockRequirements)0);

            tr::logger().debug("{},{},{}", topPos.x, topPos.y, topPos.z);

            // if (topPos.y == pos.y) {
            //     hasFound = true;
            //     break;
            // }

            // isSurface = false;
        }
        if (!hasFound) {
            //            return {"No position", true};
        }

        return {"No position", true};

        // SpawnConditions conditions{isSurface, false, false,  !isSurface,
        //                            0,         0,     bright, pos};
        // auto *bs = player->getBlockSource();
        // auto *block = bs->getBlock(pos);
        // std::map<std::string, int> spawnMap;
        // for (int i = 0; i < 200; ++i) {
        //     auto mobData = getMobToSpawn(block->getLegacy(), conditions, bs);
        //     if (mobData) spawnMap[mobData->getActorID()->getName()]++;
        // }
        // int totalCount = 0;
        // for (const auto &mob : spawnMap) totalCount += mob.second;
        // trapdoor::MessageBuilder builder;
        // builder.sTextF(trapdoor::MSG_COLOR::BOLD |
        // trapdoor::MSG_COLOR::WHITE,
        //                "-- [%d %d %d] ", pos.x, pos.y, pos.z);
        // if (isSurface) {
        //     builder.sTextF(
        //         trapdoor::MSG_COLOR::GREEN | trapdoor::MSG_COLOR::BOLD,
        //         "surface");
        // } else {
        //     builder.sTextF(trapdoor::MSG_COLOR::RED |
        //     trapdoor::MSG_COLOR::BOLD,
        //                    "underground");
        // }
        // builder.text(" --\n");
        // for (const auto &mob : spawnMap) {
        //     builder.text(mob.first)
        //         .text("    ")
        //         .num(mob.second * 100 / totalCount)
        //         .text("%%%%\n");
        // }
        // builder.send(player);
    }

}  // namespace tr
