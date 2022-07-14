#include "SpawnHelper.h"

#include <MC/ActorSpawnRuleGroup.hpp>
#include <MC/Biome.hpp>
#include <MC/Block.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Brightness.hpp>
#include <MC/Dimension.hpp>
#include <MC/Level.hpp>
#include <MC/Material.hpp>
#include <MC/Spawner.hpp>
#include <unordered_map>

#include "CommandHelper.h"
#include "DataConverter.h"
#include "HookAPI.h"
#include "Msg.h"
#include "Utils.h"
enum SpawnBlockRequirements;
class SpawnConditions;
namespace trapdoor {
    namespace {

        ActorSpawnRuleGroup *getSpawnRuleGroup() {
            return SymCall("?getSpawnRules@Level@@UEBAPEBVActorSpawnRuleGroup@@XZ",
                           ActorSpawnRuleGroup *, Level *)(Global<Level>);
        }

        bool isSpawnConditionsOK(Spawner *sp, const TMobSpawnRules *rule, BlockSource &bs,
                                 const BlockPos &pos) {
            return SymCall(
                "?_isSpawnPositionOk@Spawner@@IEBA_NAEBVMobSpawnRules@@"
                "AEAVBlockSource@@AEBVBlockPos@@_N@Z",
                bool, Spawner *, const TMobSpawnRules *, BlockSource &, const BlockPos &,
                bool)(sp, rule, bs, pos, false);
        }

        TSpawnConditions buildSpawnConditions(const BlockPos &pos, BlockSource &bs, bool surface) {
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

    ActionResult printCap(const ActorDefinitionIdentifier *id) {
        auto *g = getSpawnRuleGroup();
        if (!g) {
            return {"Error get spawn rule group", false};
        }
        int pool = g->getActorSpawnPool(*id);
        return {id->getFullName() + " ==> " + std::to_string(pool), true};
    }

    ActionResult countActors(Player * player, const std::string &type) {
        auto chPos = fromBlockPos(player->getPos().toBlockPos()).toChunkPos();
        auto entities = Level::getAllEntities();
        std::unordered_map<std::string, size_t> chunkList, allList, densityList;
        for (auto actor : entities) {
            if (!actor || actor->getDimensionId() != player->getDimensionId()) continue;
            auto actorCh = fromBlockPos(actor->getPos().toBlockPos()).toChunkPos();
            auto name = actor->getTypeName();
            allList[name]++;
            if (actorCh == chPos) {
                chunkList[name]++;
            }
            if (abs(actorCh.x - chPos.x) <= 4 && abs(actorCh.z - chPos.z) <= 4) {
                densityList[name]++;
            }
        }
        auto &res = allList;
        if (type == "chunk") res = chunkList;
        if (type == "density") res = densityList;
        TextBuilder builder;
        for (auto &i : res) {
            builder.sText(TB::GRAY, " - ")
                .textF("%s: ", trapdoor::i18ActorName(i.first).c_str())
                .num(i.second)
                .text("\n");
        }
        return {builder.get(), true};
    }

    ActionResult forceSpawn(Player * player, const ActorDefinitionIdentifier *id,
                            const BlockPos &pos) {
        auto targetPos = pos;
        if (targetPos == BlockPos::MAX) {
            targetPos = trapdoor::getLookAtPos(player);
        }
        if (targetPos == BlockPos::MAX) {
            return {"Invalid pos", false};
        }

        auto &bs = player->getRegion();
        Vec3 v(targetPos.x, targetPos.y + 1, targetPos.z);
        auto *mob =
            player->getLevel().getSpawner().spawnMob(bs, *id, nullptr, v, true, false, false);
        if (mob) {
            return {"", true};
        } else {
            return {"Failed to spawn mob", false};
        }
    }

    ActionResult printSpawnProbability(Player * player, const BlockPos &pos) {
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

        auto cond = buildSpawnConditions(topPos, player->getRegion(), isSurface);

        trapdoor::logger().debug("surf/under:{}/{} water/lava:{}/{}  bright:{}", cond.isOnSurface,
                                 cond.isUnderground, cond.isInWater, cond.isInLava,
                                 cond.rawBrightness);
        auto &block = player->getRegion().getBlock(topPos);
        std::map<std::string, std::pair<int, bool>> spawnMap;
        for (int i = 0; i < 1000; i++) {
            auto *mobData = block.getMobToSpawn(*reinterpret_cast<SpawnConditions *>(&cond),
                                                player->getRegion());
            if (!mobData) {
                return {"No mob to spawn", true};
            }
            auto &id = dAccess<ActorDefinitionIdentifier, 8>(mobData);
            auto iter = spawnMap.find(id.getIdentifier());
            if (iter == spawnMap.end()) {
                auto ok = isSpawnConditionsOK(&player->getLevel().getSpawner(),
                                              &dAccess<TMobSpawnRules, 184>(mobData),
                                              player->getRegion(), topPos + BlockPos(0, 1, 0));
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

        TextBuilder builder;
        builder
            .sTextF(TextBuilder::BOLD | TextBuilder::WHITE, "-- [%d %d %d] --\n", pos.x, pos.y,
                    pos.z)
            .text(" - Bight(Sky + Block): ")
            .sTextF(TextBuilder::GREEN, "%d\n", cond.rawBrightness)
            .text(" - Surface / Underground: ")
            .sTextF(TextBuilder::GREEN, "%d / %d\n", cond.isOnSurface, cond.isUnderground)
            .text(" - Water / Lava: ")
            .sTextF(TextBuilder::GREEN, "%d / %d\n\n", cond.isInWater, cond.isInLava);

        for (const auto &mob : spawnMap) {
            auto ok = mob.second.second;
            auto color = ok ? TB::DARK_GREEN : TB::DARK_RED;
            builder.sText(TB::GRAY, " - ")
                .textF("%s:  ", trapdoor::i18ActorName(mob.first).c_str())
                .num(mob.second.first * 100.0 / totalCount)
                .text("%%, Can: ")
                .sTextF(color | TB::BOLD, "%d\n", ok);
        }
        return {builder.get(), true};
    }
}  // namespace trapdoor