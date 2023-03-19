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
                return SuccessMsg("tick.status.normal");
            case TickingStatus::Frozen:
                return SuccessMsg("tick.status.frozen");
            case TickingStatus::Forwarding:
                return {fmt::format(tr("tick.status.forward"), info.forwardTickNum), true};
            case TickingStatus::Warp:
                return {fmt::format(tr("tick.status.warp"), info.remainWarpTick), true};
            case TickingStatus::Acc:
                return {fmt::format(tr("tick.status.acc"), info.accTime), true};
            case TickingStatus::SlowDown:
                return {fmt::format(tr("tick.status.slow"), info.slowDownTime), true};
            default:
                return ErrorMsg("tick.status.unknown");
        }
    }

    ActionResult freezeWorld() {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Frozen) {
            return ErrorMsg("tick.error.in-frozen");
        }

        info.status = TickingStatus::Frozen;
        return OperationSuccess();
    }

    ActionResult resetWorld() {
        auto &info = getTickingInfo();
        info.accTime = 1;
        info.forwardTickNum = 0;
        info.slowDownTime = 1;
        info.remainWarpTick = 0;
        info.status = TickingStatus::Normal;
        return OperationSuccess();
    }

    ActionResult forwardWorld(int gt) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Frozen || info.status == TickingStatus::Normal) {
            info.oldStatus = info.status;
            info.forwardTickNum = gt;
            info.status = TickingStatus::Forwarding;
            if (gt >= 1200) {
                trapdoor::broadcastMessage(tr("tick.forward.info.start"));
            }
            return {"", true};
        } else {
            return ErrorMsg("tick.forward.error");
        }
    }

    ActionResult warpWorld(int gt) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Normal || info.status == TickingStatus::Frozen) {
            // record old status
            info.oldStatus = info.status;
            info.remainWarpTick = gt;
            info.status = TickingStatus::Warp;
            return SuccessMsg("tick.warp.info.start");
        }
        return ErrorMsg("tick.warp.error");
    }

    ActionResult slowDownWorld(int times) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Normal) {
            if (times < 2 || times > 64) {
                return ErrorMsg(fmt::format(tr("tick.slow.error-range"), 2, 256));
            }

            info.status = TickingStatus::SlowDown;
            info.slowDownTime = times;
            trapdoor::broadcastMessage(fmt::format(tr("tick.slow.broadcast"), times), -1);

            return {"", true};
        } else {
            return ErrorMsg("tick.slow.error");
        }
    }

    ActionResult accWorld(int times) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Normal) {
            if (times < 2 || times > 100) {
                return ErrorMsg(fmt::format(tr("tick.acc.error-range"), 2, 100));
            }

            info.accTime = times;
            info.status = TickingStatus::Acc;
            trapdoor::broadcastMessage(fmt::format(tr("tick.acc.broadcast"), times), -1);
            return {"", true};
        } else {
            return ErrorMsg("tick.acc.error");
        }
    }

    ActionResult startProfiler(int rounds, SimpleProfiler::Type type) {
        if (rounds <= 0 || rounds > 12000) {
            return ErrorMsg(fmt::format(tr("prof.error.range"), 1, 12000));
        }

        auto &info = getTickingInfo();
        if (info.status != TickingStatus::Normal) {
            return ErrorMsg("prof.error.state");
        }

        if (normalProfiler().profiling) {
            return ErrorMsg("prof.error.running");
        } else {
            normalProfiler().start(rounds, type);
            return SuccessMsg("prof.info.start");
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
            return ErrorMsg("prof.pt.error.chunk");
        }

        auto *queue = reinterpret_cast<TBlockTickingQueue *>(&chunk->getTickQueue());
        if (!queue) {
            return ErrorMsg("prof.pt.error.pt-queue");
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

    void printTickMsg(const std::string &s) {
#ifdef DEV
        if (!trapdoor::normalProfiler().profiling) return;
        static std::unordered_set<std::string> set;
        auto tick = Global<Level>->getCurrentTick().t;
        auto msg = fmt::format("{} --> {}", tick, s);
        if (!set.count(msg)) {
            trapdoor::logger().debug("{} -> {}", tick, s);
            set.insert(msg);
        }
#endif
    }
}  // namespace trapdoor

/*
GameSession::tick
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

THook(void, "?tick@GameSession@@QEAAXXZ", void *level) {
    trapdoor::printTickMsg("ServerLevel.tick");
    auto &info = trapdoor::getTickingInfo();
    auto &mod = trapdoor::mod();
    if (info.status == trapdoor::TickingStatus::Normal) {
        PROF_TIMER(game_session, { original(level); })
        PROF_TIMER(trapdoor_session, {
            mod.lightTick();
            mod.heavyTick();
        })

        trapdoor::getMSPTinfo().push(time_game_session + time_trapdoor_session);
        auto &prof = trapdoor::normalProfiler();
        if (prof.profiling) {
            prof.gameSessionTickTime += (time_game_session + time_trapdoor_session);
            prof.trapdoorSessionTickTime += time_trapdoor_session;
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
            trapdoor::broadcastMessage(tr("tick.warp.info.end"), -1);
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

            trapdoor::broadcastMessage(tr("tick.forward.info.end"), -1);
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
    trapdoor::printTickMsg("LevelChunk.tick");
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
    trapdoor::printTickMsg("LevelChunk.tickBlocks");
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
    trapdoor::printTickMsg("LevelChunk.tickBlockEntities");
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
    trapdoor::printTickMsg("LevelChunk.tickPendingTicks");
    max = trapdoor::mod().getConfig().getGlobalFunctionConfig().maxPendingTickSize;
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
    trapdoor::printTickMsg("Dimension.tick");
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
    trapdoor::printTickMsg("EntitySystem.tick");
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
    trapdoor::printTickMsg("Dimension.tickRedstone");
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
    trapdoor::printTickMsg("CircuitSceneGraph.processPendingAdds");
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
    trapdoor::printTickMsg("Actor.tick");
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
