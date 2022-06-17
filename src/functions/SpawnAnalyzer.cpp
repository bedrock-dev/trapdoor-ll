//
// Created by xhy on 2022/6/16.
//

#include "SpawnAnalyzer.h"

#include <MC/ActorDefinitionIdentifier.hpp>
#include <MC/BlockSource.hpp>

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
        return {"Start analyzing", true};
    }
    ActionResult SpawnAnalyzer::stop() {
        if (!this->inAnalyzing) {
            return {"Not in analyzing", false};
        }
        this->inAnalyzing = false;
        return {"Stop analyzing", true};
    }
    void SpawnAnalyzer::collectDensityInfo() {}
    void SpawnAnalyzer::tick() {
        ++tick_count;
        if (tick_count % 20 == 0) {
            this->collectDensityInfo();
        }
    }

    ActionResult SpawnAnalyzer::printResult() const {
        TextBuilder b;
        b.textF("Total %d gt\n", this->tick_count).text("-- Surface mobs --\n");
        for (auto &p : this->surfaceMobs) {
            b.textF("- %s: %d\n", p.first.c_str(), p.second);
        }
        b.text("-- Cave mobs --\n");
        for (auto &p : this->caveMobs) {
            b.textF("- %s: %d\n", p.first.c_str(), p.second);
        }

        return {b.get(), true};
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
