#include "MCTick.h"

#include <MC/ChunkPos.hpp>
#include <MC/Dimension.hpp>
#include <MC/LevelChunk.hpp>
#include <algorithm>
#include <chrono>

#include "CommandHelper.h"
#include "HookAPI.h"
#include "LoggerAPI.h"
#include "Msg.h"
#include "SimpleProfiler.h"
#include "TrapdoorMod.h"

namespace tr {
    namespace {

        // In-file varilable;

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

    // Command Aciton

    ActionResult QueryWorld() {
        auto &info = getTickingInfo();
        switch (info.status) {
            case TickingStatus::Normal:
                return {"Normal", true};
            case TickingStatus::Frozen:
                return {"Frozen", true};
            case TickingStatus::Forwarding:
                return {fmt::format("Forwarding, {} gt left",
                                    info.forward_tick_num),
                        true};
            case TickingStatus::Wrap:
                return {
                    fmt::format("Wraping, {} gt left", info.remain_wrap_tick),
                    true};
            case TickingStatus::Acc:
                return {fmt::format("{} times faster", info.acc_time), true};
            case TickingStatus::SlowDown:
                return {fmt::format("{} times slower", info.slow_down_time),
                        true};
            default:
                return {"unknown", true};
        }
        return {"unknown", true};
    }
    ActionResult FreezeWorld() {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Frozen) {
            return {"Already in freeze status", false};
        }

        info.status = TickingStatus::Frozen;
        return {"success", true};
    }

    ActionResult ResetWorld() {
        auto &info = getTickingInfo();
        info.acc_time = 1;
        info.forward_tick_num = 0;
        info.slow_down_time = 1;
        info.remain_wrap_tick = 0;
        info.status = TickingStatus::Normal;
        return {"success", true};
    }

    ActionResult ForwardWorld(int gt) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Frozen ||
            info.status == TickingStatus::Normal) {
            info.old_status = info.status;
            info.forward_tick_num = gt;
            info.status = TickingStatus::Forwarding;
            if (gt >= 1200) {
                tr::BroadcastMessage("The world begins to forward");
                return {"Forward start", true};
            } else {
                return {"", true};
            }
        } else {
            return {"Forward can only be used on normal or freeze mode", false};
        }
    }
    ActionResult WrapWorld(int gt) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Normal ||
            info.status == TickingStatus::Frozen) {
            // record old status
            info.old_status = info.status;
            info.remain_wrap_tick = gt;
            info.status = TickingStatus::Wrap;
            return {"Warp start", true};
        }
        return {"Wrap can only be used on normal or freeze mode", false};
    }
    ActionResult SlowDownWorld(int times) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Normal) {
            if (times < 2 || times > 64) {
                return {"times show be limited in [2,64]", false};
            }
            info.status = TickingStatus::SlowDown;
            info.slow_down_time = times;
            tr::BroadcastMessage(
                fmt::format("The world will run {} times slower", times), -1);
            return {"", true};
        } else {
            return {"Slow can only be used on normal mode", false};
        }
    }

    ActionResult AccWorld(int times) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Normal) {
            if (times < 2 || times > 10) {
                return {"times show be limited in [2,10]", false};
            }
            info.acc_time = times;
            info.status = TickingStatus::Acc;
            tr::BroadcastMessage(
                fmt::format("The world will run {} times faster", times), -1);
            return {"", true};
        } else {
            return {"Acc can only be used on normal mode", false};
        }
    }

    ActionResult StartProfiler(int rounds, SimpleProfiler::Type type) {
        if (rounds <= 0 || rounds > 1200) {
            return {"Rounds show be limited in [1,1200]", false};
        }

        auto &info = getTickingInfo();
        if (info.status != TickingStatus::Normal) {
            return {"Profiling can only be performed in normal tick state",
                    false};
        }
        if (normalProfiler().profiling) {
            return {"Another profileing is running", false};
        } else {
            normalProfiler().Start(rounds, type);
            return {"Profile Start", true};
        }
    }

    ActionResult PrintMspt() {
        auto mspt = getMSPTinfo().mean();
        auto max = getMSPTinfo().max();
        auto min = getMSPTinfo().min();
        auto tps = 1000.0 / tr::micro_to_mill(mspt);
        tps = tps > 20.0 ? 20.0 : tps;

        tr::TextBuilder builder;
        builder.text(" - MSPT / TPS: ")
            .sTextF(TextBuilder::DARK_GREEN, "%.3f / %.1f\n",
                    tr::micro_to_mill(mspt), tps)
            .text(" - MIN / MAX: ")
            .sTextF(TextBuilder::DARK_GREEN, "%.3f / %.3f \n",
                    tr::micro_to_mill(min), tr::micro_to_mill(max))
            .text(" - Normal / Redstone: ");
        auto pair = getMSPTinfo().pairs();
        builder.sTextF(TextBuilder::DARK_GREEN, "%.3f / %.3f \n",
                       tr::micro_to_mill(pair.first),
                       tr::micro_to_mill(pair.second));
        return {builder.get(), true};
    }

    double getMeanMSPT() { return tr::micro_to_mill(getMSPTinfo().mean()); }

    double getMeanTPS() {
        auto tps = 1000.0 / tr::getMeanMSPT();
        return tps > 20.0 ? 20.0 : tps;
    }

}  // namespace tr

/*
ServerLevel::tick
 - Redstone
    - Dimension::tickRedstone(shouldUpdate,cacueValue,evaluate)
    - pendingUpdate
    - pendinnRemove
    - pendingAdd
 - Dimension::tick(chunk load/village)
 - entitySystem
 - Lvevl::tick
    - LevelChunk::Tick
        - blockEnties
        - randomChunk
        - Actor::tick(non global)
*/

THook(void, "?tick@ServerLevel@@UEAAXXZ", void *level) {
    auto &info = tr::getTickingInfo();
    auto &mod = tr::mod();
    if (info.status == tr::TickingStatus::Normal) {
        TIMER_START
        original(level);
        mod.LightTick();
        mod.HeavyTick();
        TIMER_END
        tr::getMSPTinfo().push(timeResult);
        auto &prof = tr::normalProfiler();
        if (prof.profiling) {
            prof.server_level_tick_time += timeResult;
            prof.current_round++;
            if (prof.current_round == prof.total_round) {
                prof.Stop();
            }
        }
        return;
        // Wrap
    } else if (info.status == tr::TickingStatus::Wrap) {
        auto mean_mspt = tr::micro_to_mill(tr::getMSPTinfo().mean());
        int max_wrap_time = static_cast<int>(45.0 / mean_mspt);
        //最快10倍速
        max_wrap_time = std::min(max_wrap_time, 10);
        //当前gt要跑的次数
        int m = std::min(max_wrap_time, info.remain_wrap_tick);
        for (int i = 0; i < m; i++) {
            info.remain_wrap_tick--;
            original(level);
            mod.LightTick();
        }
        mod.HeavyTick();
        if (info.remain_wrap_tick <= 0) {
            tr::BroadcastMessage("Wrap finished", -1);
            info.status = info.old_status;
        }
    }

    switch (info.status) {
        case tr::TickingStatus::SlowDown:
            if (info.slow_down_counter % info.slow_down_time == 0) {
                original(level);
                mod.LightTick();
                mod.HeavyTick();
            }

            info.slow_down_counter =
                (info.slow_down_counter + 1) % info.slow_down_time;
            break;

        case tr::TickingStatus::Forwarding:
            while (info.forward_tick_num > 0) {
                original(level);
                mod.LightTick();
                --info.forward_tick_num;
            }

            tr::BroadcastMessage("Froward finished", -1);
            info.status = info.old_status;
            mod.HeavyTick();
            break;
        case tr::TickingStatus::Acc:
            for (int i = 0; i < info.acc_time; i++) {
                mod.LightTick();
                original(level);
            }
            mod.HeavyTick();
            break;
        default:
            break;
    }
}

THook(void, "?tick@LevelChunk@@QEAAXAEAVBlockSource@@AEBUTick@@@Z",
      LevelChunk *chunk, void *bs, void *tick) {
    auto &prof = tr::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(chunk, bs, tick);
        TIMER_END
        prof.chunk_info.total_tick_time += timeResult;
        auto dim_id = chunk->getDimension().getDimensionId();
        auto &cp = chunk->getPosition();
        auto tpos = tr::TBlockPos2(cp.x, cp.z);
        prof.chunk_info.chunk_counter[static_cast<int>(dim_id)][tpos].push_back(
            timeResult);
    } else {
        original(chunk, bs, tick);
    }
}

THook(void, "?tickBlocks@LevelChunk@@QEAAXAEAVBlockSource@@@Z",
      LevelChunk *chunk, void *bs) {
    auto &prof = tr::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(chunk, bs);
        TIMER_END
        prof.chunk_info.random_tick_time += timeResult;
    } else {
        original(chunk, bs);
    }
}

THook(void, "?tickBlockEntities@LevelChunk@@QEAAXAEAVBlockSource@@@Z",
      void *chunk, void *bs) {
    auto &prof = tr::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(chunk, bs);
        TIMER_END
        prof.chunk_info.block_entities_tick_time += timeResult;
    } else {
        original(chunk, bs);
    }
}

THook(void,
      "?tickPendingTicks@BlockTickingQueue@@QEAA_NAEAVBlockSource@@AEBUTick@@H_"
      "N@Z",
      void *queue, void *bs, uint64_t until, int max, bool instalTick) {
    auto &prof = tr::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(queue, bs, until, max, instalTick);
        TIMER_END
        prof.chunk_info.pending_tick_time += timeResult;
    } else {
        original(queue, bs, until, max, instalTick);
    }
}

THook(void, "?tick@Dimension@@UEAAXXZ", void *dim) {
    auto &prof = tr::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(dim);
        TIMER_END
        prof.dimension_tick_time += timeResult;
    } else {
        original(dim);
    }
}

THook(void, "?tick@EntitySystems@@QEAAXAEAVEntityRegistry@@@Z", void *es,
      void *arg) {
    auto &prof = tr::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(es, arg);
        TIMER_END
        prof.entity_system_tick_time += timeResult;
    } else {
        original(es, arg);
    }
}
// redstone stuff

// signal update
THook(void, "?tickRedstone@Dimension@@UEAAXXZ", void *dim) {
    auto &prof = tr::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(dim);
        TIMER_END
        prof.redstone_info.signal_update += timeResult;
    } else {
        original(dim);
    }
}

// pending update
THook(void, "?processPendingAdds@CircuitSceneGraph@@AEAAXXZ", void *c) {
    auto &prof = tr::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(c);
        TIMER_END
        prof.redstone_info.pending_add += timeResult;
    } else {
        original(c);
    }
}

// pemding remove
THook(void, "?removeComponent@CircuitSceneGraph@@AEAAXAEBVBlockPos@@@Z",
      void *c, void *pos) {
    auto &prof = tr::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(c, pos);
        TIMER_END
        prof.redstone_info.pending_remove += timeResult;
    } else {
        original(c, pos);
    }
}

THook(void, "?tick@Actor@@QEAA_NAEAVBlockSource@@@Z", Actor *actor, void *bs) {
    auto &prof = tr::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(actor, bs);
        TIMER_END
        prof.actor_info[static_cast<int>(actor->getDimensionId())]
                       [actor->getTypeName()]
                           .time += timeResult;
        prof.actor_info[static_cast<int>(actor->getDimensionId())]
                       [actor->getTypeName()]
                           .count++;
    } else {
        original(actor, bs);
    }
}
// pending add
