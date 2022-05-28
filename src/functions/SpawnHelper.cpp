#include "SpawnHelper.h"

#include <MC/BlockSource.hpp>
#include <MC/Level.hpp>
#include <MC/LevelChunk.hpp>
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

}  // namespace tr
