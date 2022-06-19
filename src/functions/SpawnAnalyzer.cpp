//
// Created by xhy on 2022/6/16.
//

#include "SpawnAnalyzer.h"

#include <MC/ActorDefinitionIdentifier.hpp>
#include <MC/BlockSource.hpp>
#include <MC/LevelChunk.hpp>

#include "DataConverter.h"
#include "HookAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"
namespace tr {

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
            return {"Already in analyzing", false};
        }
        this->inAnalyzing = true;
        this->dimensionID = id;
        this->centerChunkPos = pos;
        return {"Start analyzing,This may produce higher MSPT", true};
    }
    ActionResult SpawnAnalyzer::stop() {
        if (!this->inAnalyzing) {
            return {"Not in analyzing", false};
        }
        this->inAnalyzing = false;
        return {"Stop analyzing", true};
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
        b.textF("Total %d gt\n", this->tick_count)
            .text("-- Surface mobs --\n")
            .text("Spawn count\n");
        for (auto &p : this->surfaceMobs) {
            b.textF(" - %s: %d\n", p.first.c_str(), p.second);
        }
        b.text("Density info\n");
        for (auto &p : this->surfaceMobsPerTick) {
            b.textF(" - %s: %.1f\n", p.first.c_str(), p.second * 1.0 / this->tick_count);
        }

        b.text("-- Cave mobs --\n").text("Spawn count\n");
        for (auto &p : this->caveMobs) {
            b.textF("- %s: %d\n", p.first.c_str(), p.second);
        }

        b.text("Density info\n");
        for (auto &p : this->caveMobs) {
            b.textF("- %s: %d.1f\n", p.first.c_str(), p.second * 1.0 / this->tick_count);
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
}  // namespace tr

THook(Mob *,
      "?spawnMob@Spawner@@QEAAPEAVMob@@AEAVBlockSource@@AEBUActorDefinitionIdentifier@@PEAVActor@@"
      "AEBVVec3@@_N44@Z",
      void *s, BlockSource &bs, ActorDefinitionIdentifier const &id, class Actor *actor,
      class Vec3 const &pos, bool natural, bool surface, bool fromSpawner) {
    auto res = original(s, bs, id, actor, pos, natural, surface, fromSpawner);
    if (res) {
        tr::mod().getSpawnAnalyzer().AddMob(res, id.getIdentifier(), surface);
    }

    return res;
}
