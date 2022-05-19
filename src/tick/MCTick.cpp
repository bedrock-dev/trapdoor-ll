#include "MCTick.h"

#include <chrono>

#include "CommandHelper.h"
#include "HookAPI.h"
#include "LoggerAPI.h"
#include "ProfileInfo.h"
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

        NormalProfiler &normalProfiler() {
            static NormalProfiler prof;
            return prof;
        }

        ActorProfiler &actorProfiler() {
            static ActorProfiler prof;
            return prof;
        }
    }  // namespace

    // Command Aciton
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
        info.status = TickingStatus::Normal;
        return {"success", true};
    }

    ActionResult ForwardWorld(int gt) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Frozen ||
            info.status == TickingStatus::Normal) {
            info.forward_tick_num = gt;
            info.status = TickingStatus::Forwarding;
            if (gt >= 1200) {
                return {"~", true};
            } else {
                return {"", true};
            }
        } else {
            return {"err", false};
        }
    }

    ActionResult SlowDownWorld(int times) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Normal) {
            info.status = TickingStatus::SlowDown;
            info.slow_down_time = times;
            return {"~", true};
        } else {
            return {"err", false};
        }
    }

    ActionResult AccWorld(int times) {
        auto &info = getTickingInfo();
        if (info.status == TickingStatus::Normal) {
            info.acc_time = times;
            info.status = TickingStatus::Acc;
            return {"~", true};
        } else {
            return {"err", false};
        }
    }

    ActionResult StartNormalProfiler(int rounds) {
        if (normalProfiler().profiling) {
            return {"err", false};
        } else {
            normalProfiler().Start(rounds);
            return {"~", true};
        }
    }

    double getMeanMSPT() { return tr::micro_to_mill(getMSPTinfo().mean()); }

    double getMeanTPS() {
        auto tps = 1000.0 / tr::getMeanMSPT();
        return tps > 20.0 ? 20.0 : tps;
    }

}  // namespace tr

THook(void, "?tick@ServerLevel@@UEAAXXZ", void *level) {
    auto &info = tr::getTickingInfo();

    if (info.status == tr::TickingStatus::Normal) {
        TIMER_START
        original(level);
        TIMER_END
        tr::getMSPTinfo().push(timeResult);
        auto &prof = tr::normalProfiler();
        prof.server_level_tick_time += timeResult;
        prof.current_round++;
        if (prof.current_round == prof.total_round) {
            prof.Stop();
        }
        return;
    }

    switch (info.status) {
        case tr::TickingStatus::SlowDown:
            if (info.slow_down_counter % info.slow_down_time == 0) {
                original(level);
            }
            info.slow_down_counter =
                (info.slow_down_counter + 1) % info.slow_down_time;
            break;

        case tr::TickingStatus::Forwarding:
            for (auto i = 0; i < info.forward_tick_num; i++) {
                original(level);
            }
            tr::ResetWorld();
            break;
        case tr::TickingStatus::Acc:
            for (int i = 0; i < info.acc_time; i++) {
                original(level);
            }
            break;
    }
}

THook(void, "?tick@LevelChunk@@QEAAXAEAVBlockSource@@AEBUTick@@@Z", void *chunk,
      void *bs, void *tick) {
    auto &prof = tr::normalProfiler();
    if (prof.profiling) {
        TIMER_START
        original(chunk, bs, tick);
        TIMER_END
        prof.chunk_info.total_tick_time += timeResult;
    } else {
        original(chunk, bs, tick);
    }
}

THook(void, "?tickBlocks@LevelChunk@@QEAAXAEAVBlockSource@@@Z", void *chunk,
      void *bs) {
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
        original(es);
        TIMER_END
        prof.entity_system_tick_time += timeResult;
    } else {
        original(es);
    }
}

// Redstone
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

// redstone stuff

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
    original(c, pos);
}
// pending add
