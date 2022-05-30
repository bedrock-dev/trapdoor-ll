#include "SpawnHelper.h"

#include <MC/Block.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Brightness.hpp>
#include <MC/Dimension.hpp>
#include <MC/Level.hpp>
#include <MC/LevelChunk.hpp>
#include <MC/Material.hpp>
#include <MC/Spawner.hpp>

#include "DataConverter.h"
#include "GlobalServiceAPI.h"
#include "Msg.h"

enum SpawnBlockRequirements;
class SpawnConditions;
namespace tr {

    namespace {

        TSpawnConditions buildSpawnConditions(const BlockPos &pos,
                                              BlockSource &bs, bool surface) {
            auto &a1m = bs.getBlock(pos + BlockPos(0, 1, 0)).getMaterial();
            auto &a2m = bs.getBlock(pos + BlockPos(0, 2, 0)).getMaterial();
            auto a1water = a1m.isType(static_cast<MaterialType>(5));
            auto a1lava = a1m.isType(static_cast<MaterialType>(6));
            auto a2water = a2m.isType(static_cast<MaterialType>(5));
            auto a2lava = a2m.isType(static_cast<MaterialType>(6));
            auto rb = bs.getRawBrightness(pos + BlockPos(0, 1, 0), true, true);
            auto bright = (uint32_t) * reinterpret_cast<unsigned char *>(&rb);
            TSpawnConditions cond;
            cond.isOnSurface = surface;
            cond.isUnderground = !surface;
            cond.isInLava = surface ? a1lava && a2lava : a1lava;
            cond.isInWater = surface ? a1water && a2water : a2water;
            cond.rawBrightness = bright;
            // if (cond.isInLava || cond.isInWater) {
            //     cond.land = ;
            // }
            cond.pos = pos;
            return cond;
        }
    }  // namespace

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
        player->getLevel().getSpawner();
        auto &dim = player->getDimension();
        //实际亮度(人工光源 + 建筑遮挡 + 天空光)
        //    auto b = player->getRegion().getRawBrightness(pos, true, true);
        //仅算建筑遮挡带来的亮度(如果在一个block下就是14(=15-1)),不管是否有人工光/天空光都是这个值
        //        auto b2 = player->getRegion().getRawBrightness(pos, true,
        //        false);
        //    auto bright = *reinterpret_cast<unsigned char *>(&b);
        //      auto bright2 = *reinterpret_cast<unsigned char *>(&b2);

        //    auto b5 = player->getRegion().getBrightness(pos);
        //   tr::logger().debug("{} {} {}", bright, bright2, b5);
        BlockPos topPos = {pos.x, 320, pos.z};
        bool isSurface = true;
        bool hasFound = false;

        int lastY = topPos.y;
        while (topPos.y >= -64) {
            Spawner::findNextSpawnBlockUnder(player->getRegion(), topPos,
                                             static_cast<MaterialType>(52),
                                             (SpawnBlockRequirements)0);
            if (topPos.y == pos.y) {
                hasFound = true;
                break;
            }
            isSurface = false;
            if (lastY == topPos.y) {
                break;
            } else {
                lastY = topPos.y;
            }
        }
        if (!hasFound) {
            return {"No position", true};
        }

        auto cond =
            buildSpawnConditions(topPos, player->getRegion(), isSurface);

        tr::logger().debug("surf/under:{}/{} water/lava:{}/{}  bright:{}",
                           cond.isOnSurface, cond.isUnderground, cond.isInWater,
                           cond.isInLava, cond.rawBrightness);
        auto &block = player->getRegion().getBlock(topPos);
        std::map<std::string, int> spawnMap;

        for (int i = 0; i < 1000; i++) {
            auto *mobData =
                block.getMobToSpawn(*reinterpret_cast<SpawnConditions *>(&cond),
                                    player->getRegion());
            if (!mobData) {
                return {"no mob to spawn", true};
            }
            auto &id = dAccess<ActorDefinitionIdentifier, 8>(mobData);
            spawnMap[id.getIdentifier()]++;
        }

        int totalCount = 0;
        for (const auto &mob : spawnMap) {
            totalCount += mob.second;
        }

        TextBuilder builder;
        builder
            .sTextF(TextBuilder::BOLD | TextBuilder::WHITE,
                    "-- [%d %d %d] --\n", pos.x, pos.y, pos.z)
            .text(" - Bight(Sky + Block): ")
            .sTextF(TextBuilder::GREEN, "%d\n", cond.rawBrightness)
            .text(" - Surface / UnderGroudn: ")
            .sTextF(TextBuilder::GREEN, "%d / %d\n\n", cond.isOnSurface,
                    cond.isUnderground)
            .text(" - Water / Lava: ")
            .sTextF(TextBuilder::GREEN, "%d / %d\n", cond.isInWater,
                    cond.isInLava);

        for (const auto &mob : spawnMap) {
            builder.textF("- %s: ", mob.first.c_str())
                .num(mob.second * 100 / totalCount)
                .text("%%\n");
        }
        return {builder.get(), true};
    }

}  // namespace tr
   /*
   0X1A1A6C0
   public: class MobSpawnerData const * __ptr64 __cdecl
   Block::getMobToSpawn(class    SpawnConditions const & __ptr64,class BlockSource
   & __ptr64)const __ptr64
   ?getMobToSpawn@Block@@QEBAPEBVMobSpawnerData@@AEBVSpawnConditions@@AEAVBlockSource@@@Z
   
   */

// struct SPC {
//     bool surface;
//     bool water;
//     char g1[1];
//     bool lava;
//     bool underground;
//     char g2[15];
//     BlockPos pos;
// };
// THook(void *,
//       "?getMobToSpawn@Block@@QEBAPEBVMobSpawnerData@@AEBVSpawnConditions@@"
//       "AEAVBlockSource@@@Z",
//       Block *block, SPC *cond, void *bs) {
//     printf("%d,%d,%d => sur: %d water: %d und: %d, ", cond->pos.x,
//     cond->pos.y,
//            cond->pos.z, cond->surface, cond->water, cond->underground);
//     for (int i = 0; i < 2; i++) {
//         printf("%02d ", (int)cond->g1[i]);
//     }

//     printf("\n");
//     return original(block, cond, bs);
// }