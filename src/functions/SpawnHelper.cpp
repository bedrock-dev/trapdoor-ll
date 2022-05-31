#include "SpawnHelper.h"

#include <MC/Biome.hpp>
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
#include "HookAPI.h"
#include "Msg.h"

enum SpawnBlockRequirements;
class SpawnConditions;
namespace tr {

    namespace {
        bool IsSpawnConditionsOK(Spawner *sp, const TMobSpawnRules *rule,
                                 BlockSource &bs, const BlockPos &pos) {
            return SymCall(
                "?_isSpawnPositionOk@Spawner@@IEBA_NAEBVMobSpawnRules@@"
                "AEAVBlockSource@@AEBVBlockPos@@_N@Z",
                bool, Spawner *, const TMobSpawnRules *, BlockSource &,
                const BlockPos &, bool)(sp, rule, bs, pos, false);
        }

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
            cond.pos = pos + BlockPos(0, 1, 0);
            return cond;
        }
    }  // namespace

    ActionResult PrintCap() {
        auto &sp = Global<Level>->getSpawner();
        return {"", true};
    }
    ActionResult SpawnMobCluster(Player *p, const BlockPos &pos) {
        return {"~", true};
    }

    ActionResult CountActors(Player *p, const std::string &type) {
        return {"", true};
    }
    ActionResult ForceSpawn(Player *p, const ActorDefinitionIdentifier *id,
                            const BlockPos &pos) {
        auto &bs = p->getRegion();
        Vec3 v(pos.x, pos.y + 1, pos.z);
        auto *mob = p->getLevel().getSpawner().spawnMob(bs, *id, nullptr, v,
                                                        true, false, false);

        return {"", mob != nullptr};
    }

    ActionResult printSpawnProbability(Player *player, const BlockPos &pos) {
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
        std::map<std::string, std::pair<int, bool>> spawnMap;
        for (int i = 0; i < 1000; i++) {
            auto *mobData =
                block.getMobToSpawn(*reinterpret_cast<SpawnConditions *>(&cond),
                                    player->getRegion());
            if (!mobData) {
                return {"no mob to spawn", true};
            }
            auto &id = dAccess<ActorDefinitionIdentifier, 8>(mobData);
            auto iter = spawnMap.find(id.getIdentifier());
            if (iter == spawnMap.end()) {
                auto ok = IsSpawnConditionsOK(
                    &player->getLevel().getSpawner(),
                    &dAccess<TMobSpawnRules, 184>(mobData), player->getRegion(),
                    topPos + BlockPos(0, 1, 0));
                spawnMap[id.getIdentifier()] = {1, ok};
            } else {
                auto ct = iter->second.first;
                auto ok = iter->second.second;
                spawnMap[id.getIdentifier()] = {ct + 1, ok};
            }
        }

        int totalCount = 0;
        for (const auto &mob : spawnMap) {
            totalCount += mob.second.first;
        }

        // auto biome = player->getRegion().getBiome()`
        TextBuilder builder;
        builder
            .sTextF(TextBuilder::BOLD | TextBuilder::WHITE,
                    "-- [%d %d %d] --\n", pos.x, pos.y, pos.z)
            .text(" - Bight(Sky + Block): ")
            .sTextF(TextBuilder::GREEN, "%d\n", cond.rawBrightness)
            .text(" - Surface / UnderGroudn: ")
            .sTextF(TextBuilder::GREEN, "%d / %d\n", cond.isOnSurface,
                    cond.isUnderground)
            .text(" - Water / Lava: ")
            .sTextF(TextBuilder::GREEN, "%d / %d\n\n", cond.isInWater,
                    cond.isInLava);

        for (const auto &mob : spawnMap) {
            auto ok = mob.second.second;
            auto color = ok ? TextBuilder::DARK_GREEN : TextBuilder::DARK_RED;
            builder.textF("- %s: ", mob.first.c_str())
                .sTextF(color, "%.2f%%\n",
                        mob.second.first * 100.0 / totalCount);
        }

        return {builder.get(), true};
    }

}  // namespace tr

// THook(bool,
//       "?_isSpawnPositionOk@Spawner@@IEBA_NAEBVMobSpawnRules@@"
//       "AEAVBlockSource@@AEBVBlockPos@@_N@Z",
//       Spawner *sp, const tr::TMobSpawnRules *r, BlockSource &bs,
//       const BlockPos &p, bool a) {
//     tr::logger().debug("{} {} {}", p.x, p.y, p.z);
//     return original(sp, r, bs, p, a);
// }