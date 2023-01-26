//
// Created by xhy on 2022/6/16.
//

#include "SpawnAnalyzer.h"

#include <mc/ActorDefinitionIdentifier.hpp>
#include <mc/BlockSource.hpp>
#include <mc/LevelChunk.hpp>

#include "DataConverter.h"
#include "HookAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"
#include "Utils.h"
namespace trapdoor {
    void SpawnAnalyzer::AddMob(Mob *mob, const string &name, bool surface) {
        if (!inAnalyzing || !mob) return;
        auto dimension = mob->getDimensionId();
        if (dimension != dimensionID) return;
        if (surface)
            this->surfaceMobs[name]++;
        else
            this->caveMobs[name]++;
    }

    ActionResult SpawnAnalyzer::start(int id, const TBlockPos2 &pos) {
        if (this->inAnalyzing) {
            return ErrorMsg("spawn.analyzer.error.start");
        }
        this->clear();
        this->inAnalyzing = true;
        this->dimensionID = id;
        this->centerChunkPos = pos;
        return SuccessMsg("spawn.analyzer.success.start");
    }

    ActionResult SpawnAnalyzer::stop() {
        if (!this->inAnalyzing) {
            return ErrorMsg("spawn.analyzer.error.stop");
        }
        this->inAnalyzing = false;
        return SuccessMsg("spawn.analyzer.success.stop");
    }

    void SpawnAnalyzer::collectDensityInfo() {
        if (!this->inAnalyzing) return;
        auto entities = Level::getAllEntities();
        for (auto actor : entities) {
            if (!actor || actor->getDimensionId() != this->dimensionID) continue;
            auto actorCh = fromBlockPos(actor->getPos().toBlockPos()).toChunkPos();
            auto name = actor->getTypeName();
            if (abs(actorCh.x - this->centerChunkPos.x) <= 4 &&
                abs(actorCh.z - this->centerChunkPos.z) <= 4) {
                if (actor->isSurfaceMob()) {
                    this->surfaceMobsPerTick[name]++;
                } else {
                    this->caveMobsPerTick[name]++;
                }
            }
        }
    }

    void SpawnAnalyzer::tick() {
        ++tick_count;
        this->collectDensityInfo();
    }

    ActionResult SpawnAnalyzer::printResult() const {
        TextBuilder b;
        // b.textF("Total %d gt\n", this->tick_count)
        const float hour = static_cast<float>(tick_count) / 72000.0f;
        b.text("Total ").num(tick_count).text("gt (").num(hour).text("h)\n");
        b.sText(TB::GREEN | TB::BOLD, "Surface:\n");
        for (auto &p : this->surfaceMobsPerTick) {
            auto type = trapdoor::rmmc(p.first);
            if (type == "player") continue;
            b.sText(TB::GRAY, " - ").textF("%s:  ", trapdoor::i18ActorName(type).c_str());
            b.text("Density: ").num(static_cast<float>(p.second) / static_cast<float>(tick_count));
            auto iter = this->surfaceMobs.find(type);
            if (iter == this->surfaceMobs.end()) {
                b.text("\n");
            } else {
                b.text(" Spawn: ")
                    .num(iter->second)
                    .text(", ")
                    .num(static_cast<float>(iter->second) / hour)
                    .text("/h\n");
            }
        }
        b.sText(TB::RED | TB::BOLD, "Underground:\n");
        for (auto &p : this->caveMobsPerTick) {
            auto type = trapdoor::rmmc(p.first);
            if (type == "player") continue;
            b.sText(TB::GRAY, " - ").textF("%s:  ", trapdoor::i18ActorName(type).c_str());
            b.text("Density: ").num(static_cast<float>(p.second) / static_cast<float>(tick_count));
            auto iter = this->caveMobs.find(type);
            if (iter == this->caveMobs.end()) {
                b.text("\n");
            } else {
                b.text(" Spawn: ")
                    .num(iter->second)
                    .text(", ")
                    .num(static_cast<float>(iter->second) / hour)
                    .text("/h\n");
            }
        }
        return {b.get(), true};
    }
    ActionResult SpawnAnalyzer::clear() {
        this->tick_count = 0;
        this->surfaceMobs.clear();
        this->caveMobs.clear();
        this->surfaceMobsPerTick.clear();
        this->caveMobsPerTick.clear();
        return {"Cleared", true};
    }
}  // namespace trapdoor

THook(Mob *,
      "?spawnMob@Spawner@@QEAAPEAVMob@@AEAVBlockSource@@AEBUActorDefinitionIdentifier@@PEAVActor@@"
      "AEBVVec3@@_N44@Z",
      void *s, BlockSource &bs, ActorDefinitionIdentifier const &id, class Actor *actor,
      class Vec3 const &pos, bool natural, bool surface, bool fromSpawner) {
    auto res = original(s, bs, id, actor, pos, natural, surface, fromSpawner);
    if (res) {
        trapdoor::mod().getSpawnAnalyzer().AddMob(res, id.getIdentifier(), surface);
    }

    return res;
}
