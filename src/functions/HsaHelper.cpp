#include "HsaHelper.h"

#include <mc/Biome.hpp>
#include <mc/Block.hpp>
#include <mc/BlockPos.hpp>
#include <mc/BlockSource.hpp>
#include <mc/ChunkSource.hpp>
#include <mc/Dimension.hpp>
#include <mc/ItemStack.hpp>
#include <mc/LevelChunk.hpp>

#include "DataConverter.h"
#include "HookAPI.h"
#include "Msg.h"
#include "Particle.h"
#include "Utils.h"

namespace trapdoor {
    namespace {
        TAABB getSpawnAreaFromHSA(const BoundingBox &aabb) {
            int x = (aabb.max.x - aabb.min.x + 1) / 2 + aabb.min.x;
            int z = (aabb.max.z - aabb.min.z + 1) / 2 + aabb.min.z + 1;
            TVec3 minPoint{x, aabb.min.y, z - 1};
            TVec3 maxPoint{x + 1, aabb.max.y + 1, z};
            return {minPoint, maxPoint};
        }

        std::string hsaTypeToStr(LevelChunk::HardcodedSpawnAreaType type) {
            TextBuilder builder;
            if (type == LevelChunk::HardcodedSpawnAreaType::NETHER_FORTRESS) {
                builder.sText(TB::GREEN, "Nether fortress");
            } else if (type == LevelChunk::HardcodedSpawnAreaType::SWAMP_HUT) {
                builder.sText(TB::RED, "Swamp hut");
            } else if (type == LevelChunk::HardcodedSpawnAreaType::PILLAGER_OUTPOST) {
                builder.sText(TB::BLUE, "Pillager outpost");
            } else if (type == LevelChunk::HardcodedSpawnAreaType::OCEAN_MONUMENT) {
                builder.sText(TB::YELLOW, "Ocean monument");
            } else {
                builder.sText(TB::GRAY, "Unknown");
            }
            return builder.get();
        }
    }  // namespace

    //  bool HsaInfo::operator<(const HsaInfo &rhs) const { return bb < rhs.bb; }

    void HsaManager::HeavyTick() {
        if (!this->showHsa) return;
        static int gt = 0;

        if (gt % 40 == 0) {
            //            Global<Level>->forEachDimension([&globalAreas](Dimension &d) {
            //                auto map = d.getChunkSource().getChunkMap();
            //                if (map) {
            //                    for (auto &kv : *map) {
            //                        auto *chunk = kv.second.lock().get();
            //                        if (chunk /*&& chunk->getState().load() ==
            //                        static_cast<ChunkState>(0x202)*/) {
            //                            trapdoor::logger().debug("chunk state is {:x} ",
            //                                                     static_cast<int>(chunk->getState().load()));
            //                            auto &HSAs = chunk->getSpawningAreas();
            //                            for (auto &hsa : HSAs) {
            //                                globalAreas[hsa.aabb.min] = hsa;
            //                            }
            //                        }
            //                    }
            //                }
            //                return true;
            //            });
            this->globalAreas.clear();
            const int radius = 5;
            Global<Level>->forEachPlayer([this, radius](Player &p) {
                auto cp = trapdoor::blockPosToChunkPos(p.getBlockPos());
                for (int i = -radius; i <= radius; i++) {
                    for (int j = -radius; j <= radius; j++) {
                        auto chunk = p.getRegion().getChunk(cp.x + i, cp.z + j);
                        if (chunk && chunk->isFullyLoaded()) {
                            auto &HSAs = chunk->getSpawningAreas();
                            for (auto &hsa: HSAs) {
                                // 同一个右下角应该只有一个
                                globalAreas[hsa.aabb.min] = hsa;
                            }
                        }
                    }
                }
                return true;
            });

            // trapdoor::logger().debug("Total {} hsa found", globalAreas.size());
            // 缓存完成后开始显示
            auto color = PCOLOR::WHITE;
            for (const auto &hsa: globalAreas) {
                switch (hsa.second.type) {
                    case LevelChunk::HardcodedSpawnAreaType::PILLAGER_OUTPOST:
                        color = PCOLOR::VATBLUE;
                        break;
                    case LevelChunk::HardcodedSpawnAreaType::SWAMP_HUT:
                        color = PCOLOR::RED;
                        break;
                    case LevelChunk::HardcodedSpawnAreaType::NETHER_FORTRESS:
                        color = PCOLOR::GREEN;
                        break;
                    case LevelChunk::HardcodedSpawnAreaType::OCEAN_MONUMENT:
                        color = PCOLOR::YELLOW;
                        break;
                    default:
                        break;
                }

                auto dimId =
                        hsa.second.type == LevelChunk::HardcodedSpawnAreaType::NETHER_FORTRESS ? 1 : 0;
                trapdoor::drawAABB(getSpawnAreaFromHSA(hsa.second.aabb), color, true, dimId);
            }
        }
        gt = (gt + 1) % 80;
    }

    ActionResult HsaManager::place(Player *player, bool replaceAirOnly) {
        if (!player) return trapdoor::ErrorPlayerNeed();
        if (player->getPlayerGameType() != GameType::Creative)
            return trapdoor::ErrorCreativeNeed();

        auto block = player->getHandSlot()->getBlock();
        if (!block) return trapdoor::OperationSuccess();
        trapdoor::logger().debug("Use block: {}", block->getName().getString());
        for (const auto &hsa: globalAreas) {
            trapdoor::logger().debug("hsa!");
            auto &aabb = hsa.second.aabb;
            auto startY = aabb.min.y;
            auto endY = aabb.max.y;
            int x = (aabb.max.x - aabb.min.x + 1) / 2 + aabb.min.x;
            int z = (aabb.max.z - aabb.min.z + 1) / 2 + aabb.min.z;
            for (int y = startY; y <= endY; y++) {
                if (!replaceAirOnly || player->getRegion().getBlock(x, y, z).isAir()) {
                    // trapdoor::logger().debug("set block!!");
                    player->getRegion().setBlock(x, y, z, *block, 3, nullptr);
                }
            }
        }
        return OperationSuccess();
    }

    ActionResult HsaManager::count() {
        std::unordered_map<LevelChunk::HardcodedSpawnAreaType, int> counter;
        for (auto &kv: this->globalAreas) {
            counter[kv.second.type]++;
        }

        trapdoor::TextBuilder builder;
        for (auto &kv: counter) {
            builder.sText(TB::GRAY, " - ")
                    .text(hsaTypeToStr(kv.first))
                    .textF(" %d\n", kv.second)
                    .endl();
        }
        return SuccessMsg(builder.removeEndl().get());
    }
}  // namespace trapdoor
