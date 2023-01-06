#include "MCTick.h"

#include <algorithm>
#include <chrono>
#include <mc/ChunkPos.hpp>
#include <mc/Dimension.hpp>
#include <mc/LevelChunk.hpp>
#include <mc/Vec3.hpp>

#include "CommandHelper.h"
#include "DataConverter.h"
#include "HookAPI.h"
#include "LoggerAPI.h"
#include "Msg.h"
#include "SimpleProfiler.h"
#include "TrapdoorMod.h"
#include "Utils.h"
namespace trapdoor {
    namespace {

        // In-file variable;

        TickingInfo &getTickingInfo() {
            static TickingInfo info;
            return info;
        }

        MSPTInfo &getMSPTinfo() {
            static MSPTInfo info;
            return info;
        }

        SimpleProfiler &normalProfiler() {
            static SimpleProfiler prof;
            return prof;
        }

    }  // namespace

    // Command Action

    ActionResult queryWorld() {
        auto &info = getTickingInfo();
        switch (info.status) {
            case TickingStatus::Normal:
                return {"Normal", true};
            case TickingStatus::Frozen:
                return {"Frozen", true};
            case TickingStatus::Forwarding:
                return {fmt::format("Forwarding, {} gt left", info.forwardTickNum), true};
            case TickingStatus::Warp:
                return {fmt::format("Wrapping, {} gt left", info.remainWarpTick), true};
            case TickingStatus::Acc:
                return {fmt::format("{} times faster", info.accTime), true};
            case TickingStatus::SlowDown:
                return {fmt::format("{} times slower", info.slowDownTime), true};
            default:
                return {"unknown", true};
        }
    }
    ActionResult freezeWorld() {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Frozen) {
            return {"Already in freeze status", false};
        }

        info.status = TickingStatus::Frozen;
        return {"Success", true};
    }

    ActionResult resetWorld() {
        auto &info = getTickingInfo();
        info.accTime = 1;
        info.forwardTickNum = 0;
        info.slowDownTime = 1;
        info.remainWarpTick = 0;
        info.status = TickingStatus::Normal;
        return {"Success", true};
    }

    ActionResult forwardWorld(int gt) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Frozen || info.status == TickingStatus::Normal) {
            info.oldStatus = info.status;
            info.forwardTickNum = gt;
            info.status = TickingStatus::Forwarding;
            if (gt >= 1200) {
                trapdoor::broadcastMessage("The world begins to forward");
                return {"Forward start", true};
            } else {
                return {"", true};
            }
        } else {
            return {"Forward can only be used on normal or freeze status", false};
        }
    }
    ActionResult warpWorld(int gt) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Normal || info.status == TickingStatus::Frozen) {
            // record old status
            info.oldStatus = info.status;
            info.remainWarpTick = gt;
            info.status = TickingStatus::Warp;
            return {"Warp start", true};
        }
        return {"Warp can only be used on normal or freeze status", false};
    }
    ActionResult slowDownWorld(int times) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Normal) {
            if (times < 2 || times > 64) {
                return {"times show be limited in [2,64]", false};
            }
            info.status = TickingStatus::SlowDown;
            info.slowDownTime = times;
            trapdoor::broadcastMessage(fmt::format("The world will run {} times slower", times),
                                       -1);
            return {"", true};
        } else {
            return {"Slow can only be used on normal mode", false};
        }
    }

    ActionResult accWorld(int times) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Normal) {
            if (times < 2 || times > 100) {
                return ErrorRange("acc time", 2, 100);
            }
            info.accTime = times;
            info.status = TickingStatus::Acc;
            trapdoor::broadcastMessage(fmt::format("The world will run {} times faster", times),
                                       -1);
            return {"", true};
        } else {
            return {"Acc can only be used on normal mode", false};
        }
    }

    ActionResult startProfiler(int rounds, SimpleProfiler::Type type) {
        if (rounds <= 0 || rounds > 12000) {
            return {"Rounds show be limited in [1,12000]", false};
        }

        auto &info = getTickingInfo();
        if (info.status != TickingStatus::Normal) {
            return {"Profiling can only be performed in normal tick state", false};
        }
        if (normalProfiler().profiling) {
            return {"Another profiling is running", false};
        } else {
            normalProfiler().start(rounds, type);
            return {"Profile Start", true};
        }
    }

    ActionResult printMSPT() {
        auto mspt = getMSPTinfo().mean();
        auto max = getMSPTinfo().max();
        auto min = getMSPTinfo().min();
        auto tps = 1000.0 / trapdoor::micro_to_mill(mspt);
        tps = tps > 20.0 ? 20.0 : tps;

        trapdoor::TextBuilder builder;
        builder.text(" - MSPT / TPS: ")
            .sTextF(TextBuilder::DARK_GREEN, "%.3f / %.1f\n", trapdoor::micro_to_mill(mspt), tps)
            .text(" - MIN / MAX: ")
            .sTextF(TextBuilder::DARK_GREEN, "%.3f / %.3f \n", trapdoor::micro_to_mill(min),
                    trapdoor::micro_to_mill(max))
            .text(" - Normal / Redstone: ");
        auto pair = getMSPTinfo().pairs();
        builder.sTextF(TextBuilder::DARK_GREEN, "%.3f / %.3f \n",
                       trapdoor::micro_to_mill(pair.first), trapdoor::micro_to_mill(pair.second));
        return {builder.get(), true};
    }

    double getMeanMSPT() { return trapdoor::micro_to_mill(getMSPTinfo().mean()); }

    double getMeanTPS() {
        auto tps = 1000.0 / trapdoor::getMeanMSPT();
        return tps > 20.0 ? 20.0 : tps;
    }
    ActionResult getPendingTickInfo(Player *player, const BlockPos &position) {
        if (!player) return trapdoor::ErrorPlayerNeed();
        bool printAll = false;
        auto p = position;

        if (p == BlockPos::MAX) {
            p = player->getPosition().toBlockPos();
            printAll = true;
        }

        auto chunkPos = fromBlockPos(p).toChunkPos();
        auto chunk = player->getBlockSource()->getChunk(chunkPos.x, chunkPos.z);
        if (!chunk) {
            return ErrorMsg("Can not get chunk info");
        }

        auto *queue = reinterpret_cast<TBlockTickingQueue *>(&chunk->getTickQueue());
        if (!queue) {
            return ErrorMsg("can not get block ticking queue");
        }

        auto buildPtItem = [](const trapdoor::TBlockTick &pt, size_t currentTick) -> std::string {
            trapdoor::TextBuilder b;
            b.text(" - ")
                .sTextF(trapdoor::TB::AQUA, "[%d %d %d]", pt.data.pos.x, pt.data.pos.y,
                        pt.data.pos.z)
                .textF(" %s ", trapdoor::rmmc(pt.data.block->getName().getString()).c_str())
                .sTextF(trapdoor::TB::GREEN, " %zu / %d ", pt.data.tick, pt.data.tick - currentTick)
                .text(" p: ")
                .sTextF(trapdoor::TB::GREEN, "%d", pt.data.priorityOffset)
                .text(" v: ")
                .sTextF(trapdoor::TB::GREEN, "%d", !pt.removed);
            return b.get();
        };

        trapdoor::TextBuilder b;
        if (printAll) {
            b.sTextF(trapdoor::TB::AQUA, "-- [%d %d] Tick = %zu, Total %d Pts--\n", chunkPos.x,
                     chunkPos.z, queue->currentTick, queue->next.queue.size());
            for (auto &pt : queue->next.queue) {
                b.textF("%s\n", buildPtItem(pt, queue->currentTick).c_str());
            }

        } else {
            for (auto &pt : queue->next.queue) {
                if (pt.data.pos == p) {
                    b.textF("%s\n", buildPtItem(pt, queue->currentTick).c_str());
                    break;
                }
            }
        }

        return {b.get(), true};
    }

}  // namespace trapdoor

/*
ServerLevel::tick
 - Redstone
    - Dimension::tickRedstone(shouldUpdate,cacheValue,evaluate)
    - pendingUpdate
    - pendingRemove
    - pendingAdd
 - Dimension::tick(chunk load/village)
 - entitySystem
 - Level::tick
    - LevelChunk::Tick
        - blockEntities
        - randomChunk
        - Actor::tick(non global)
*/

THook(void, "?tick@ServerLevel@@UEAAXXZ", void *level) {
    auto &info = trapdoor::getTickingInfo();
    auto &mod = trapdoor::mod();
    if (info.status == trapdoor::TickingStatus::Normal) {
        TIMER_START
        original(level);
        mod.lightTick();
        mod.heavyTick();
        TIMER_END
        trapdoor::getMSPTinfo().push(timeResult);
        auto &prof = trapdoor::normalProfiler();
        if (prof.profiling) {
            prof.serverLevelTickTime += timeResult;
            prof.currentRound++;
            if (prof.currentRound == prof.totalRound) {
                prof.stop();
            }
        }
        return;
        // Warp
    } else if (info.status == trapdoor::TickingStatus::Warp) {
        auto mean_mspt = trapdoor::micro_to_mill(trapdoor::getMSPTinfo().mean());
        int max_wrap_time = static_cast<int>(45.0 / mean_mspt);
        // 最快10倍速
        max_wrap_time = std::min(max_wrap_time, 10000);
        // 当前gt要跑的次数
        int m = std::min(max_wrap_time, info.remainWarpTick);
        for (int i = 0; i < m; i++) {
            info.remainWarpTick--;
            original(level);
            mod.lightTick();
        }
        mod.heavyTick();
        if (info.remainWarpTick <= 0) {
            trapdoor::broadcastMessage("Warp finished", -1);
            info.status = info.oldStatus;
        }
    }

    switch (info.status) {
        case trapdoor::TickingStatus::SlowDown:
            if (info.slowDownCounter % info.slowDownTime == 0) {
                original(level);
                mod.lightTick();
                mod.heavyTick();
            }

            info.slowDownCounter = (info.slowDownCounter + 1) % info.slowDownTime;
            break;

        case trapdoor::TickingStatus::Forwarding:
            while (info.forwardTickNum > 0) {
                original(level);
                mod.lightTick();
                --info.forwardTickNum;
            }

            trapdoor::broadcastMessage("Froward finished", -1);
            info.status = info.oldStatus;
            mod.heavyTick();
            break;
        case trapdoor::TickingStatus::Acc:
            for (int i = 0; i < info.accTime; i++) {
                mod.lightTick();
                original(level);
            }
            mod.heavyTick();
            break;
        default:
            break;
    }
}

THook(void, "?tick@LevelChunk@@QEAAXAEAVBlockSource@@AEBUTick@@@Z", LevelChunk *chunk, void *bs,
      void *tick) {
    auto &prof = trapdoor::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(chunk, bs, tick);
        TIMER_END
        prof.chunkInfo.totalTickTime += timeResult;
        auto dim_id = chunk->getDimension().getDimensionId();
        auto &cp = chunk->getPosition();
        auto tpos = trapdoor::TBlockPos2(cp.x, cp.z);
        prof.chunkInfo.chunk_counter[static_cast<int>(dim_id)][tpos].push_back(timeResult);
    } else {
        original(chunk, bs, tick);
    }
}

THook(void, "?tickBlocks@LevelChunk@@QEAAXAEAVBlockSource@@@Z", LevelChunk *chunk, void *bs) {
    auto &prof = trapdoor::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(chunk, bs);
        TIMER_END
        prof.chunkInfo.randomTickTime += timeResult;
    } else {
        original(chunk, bs);
    }
}

THook(void, "?tickBlockEntities@LevelChunk@@QEAAXAEAVBlockSource@@@Z", void *chunk, void *bs) {
    auto &prof = trapdoor::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(chunk, bs);
        TIMER_END
        prof.chunkInfo.blockEntitiesTickTime += timeResult;
    } else {
        original(chunk, bs);
    }
}

THook(bool,
      "?tickPendingTicks@BlockTickingQueue@@QEAA_NAEAVBlockSource@@AEBUTick@@H_"
      "N@Z",
      trapdoor::TBlockTickingQueue *queue, BlockSource *bs, uint64_t until, int max,
      bool instalTick) {
    max = trapdoor::mod().getConfig().getTweakConfig().maxPendingTickSize;
    auto &prof = trapdoor::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        if (!queue->next.queue.empty()) {
            // 记录PT数据
            auto tickData = queue->next.queue[0].data;
            auto chunkPos = trapdoor::fromBlockPos(tickData.pos).toChunkPos();
            auto dimId = static_cast<int>(bs->getDimension().getDimensionId());

            auto current = prof.ptCounter[dimId][chunkPos];
            prof.ptCounter[dimId][chunkPos] = std::max(current, queue->next.queue.size());
        }

        auto res = original(queue, bs, until, max, instalTick);
        TIMER_END
        prof.chunkInfo.pendingTickTime += timeResult;
        return res;
    } else {
        return original(queue, bs, until, max, instalTick);
    }
}

THook(void, "?tick@Dimension@@UEAAXXZ", void *dim) {
    auto &prof = trapdoor::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(dim);
        TIMER_END
        prof.dimensionTickTime += timeResult;
    } else {
        original(dim);
    }
}

THook(void, "?tick@EntitySystems@@QEAAXAEAVEntityRegistry@@@Z", void *es, void *arg) {
    auto &prof = trapdoor::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(es, arg);
        TIMER_END
        prof.entitySystemTickTime += timeResult;
    } else {
        original(es, arg);
    }
}
// redstone stuff

// signal update
THook(void, "?tickRedstone@Dimension@@UEAAXXZ", void *dim) {
    auto &prof = trapdoor::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(dim);
        TIMER_END
        prof.redstoneInfo.signalUpdate += timeResult;
    } else {
        original(dim);
    }
}

// pending update
THook(void, "?processPendingAdds@CircuitSceneGraph@@AEAAXXZ", void *c) {
    auto &prof = trapdoor::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(c);
        TIMER_END
        prof.redstoneInfo.pendingAdd += timeResult;
    } else {
        original(c);
    }
}

// pending remove
THook(void, "?removeComponent@CircuitSceneGraph@@AEAAXAEBVBlockPos@@@Z", void *c, void *pos) {
    auto &prof = trapdoor::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(c, pos);
        TIMER_END
        prof.redstoneInfo.pendingRemove += timeResult;
    } else {
        original(c, pos);
    }
}

THook(void, "?tick@Actor@@QEAA_NAEAVBlockSource@@@Z", Actor *actor, void *bs) {
    auto &prof = trapdoor::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(actor, bs);
        TIMER_END

        prof.actorInfo[static_cast<int>(actor->getDimensionId())][actor->getTypeName()].time +=
            timeResult;
        prof.actorInfo[static_cast<int>(actor->getDimensionId())][actor->getTypeName()].count++;
    } else {
        original(actor, bs);
    }
}
// pending add
