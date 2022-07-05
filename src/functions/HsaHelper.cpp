#include "HsaHelper.h"

#include <MC/Biome.hpp>
#include <MC/BlockSource.hpp>

#include "HookAPI.h"
#include "Particle.h"

namespace tr {

    namespace {
        TAABB getSpawnAreaFromHSA(const TBoundingBox &aabb) {
            int x = (aabb.maxPos.x - aabb.minPos.x + 1) / 2 + aabb.minPos.x;
            int z = (aabb.maxPos.z - aabb.minPos.z + 1) / 2 + aabb.minPos.z + 1;
            TVec3 minPoint{x, aabb.minPos.y, z - 1};
            TVec3 maxPoint{x + 1, aabb.maxPos.y + 1, z};
            return {minPoint, maxPoint};
        }
    }  // namespace

    bool HsaInfo::operator<(const HsaInfo &rhs) const { return bb < rhs.bb; }

    ActionResult HsaManager::place() { return {"", true}; }

    void HsaManager::HeavyTick() {
        if (!this->showHsa) return;
        static int gt = 0;
        if (gt % 40 == 0) {
            auto color = PCOLOR::WHITE;
            for (const auto &hsa : this->hsaList) {
                switch (hsa.type) {
                    case PillagerOutpost:
                        color = PCOLOR::BLUE;
                        break;
                    case SwampHut:
                        color = PCOLOR::RED;
                        break;
                    case NetherFortress:
                        color = PCOLOR::GREEN;
                        break;
                    case OceanMonument:
                        color = PCOLOR::YELLOW;
                        break;
                    default:
                        break;
                }
                tr::drawAABB(getSpawnAreaFromHSA(hsa.bb), color, true, hsa.dimensionID);
            }
        }
        gt = (gt + 1) % 80;
    }

}  // namespace tr

THook(void,
      "?_spawnStructureMob@Spawner@@AEAAXAEAVBlockSource@@AEBVBlockPos@@"
      "AEBUHardcodedSpawningArea@LevelChunk@@AEBVSpawnConditions@@@Z",
      void *spawner, const BlockSource &bs, const BlockPos &blockPos, const tr::TBoundingBox &hsa,
      void *spawnConditions) {
    original(spawner, bs, blockPos, hsa, spawnConditions);
    auto &hsaManager = tr::mod().getHsaManager();
    tr::HsaInfo info;
    info.dimensionID = bs.getDimensionId();
    info.bb = hsa;
    if (info.dimensionID == 1) {
        info.type = tr::NetherFortress;
    } else {
        auto biome = bs.tryGetBiome(blockPos);
        if (biome) {
            auto type = static_cast<int>(biome->getBiomeType());
            if (type == 10) {
                info.type = tr::OceanMonument;
            } else if (type == 15) {
                info.type = tr::SwampHut;
            } else {
                info.type = tr::PillagerOutpost;
            }
        }
    }

    hsaManager.insert(info);
}