#include "MCTick.h"

#include <chrono>

#include "CommandHelper.h"
#include "HookAPI.h"
#include "LoggerAPI.h"
#include "ProfileInfo.h"
#include "TrapdoorMod.h"

namespace tr {
    namespace {

        TickingInfo &getTickingInfo() {
            static TickingInfo info;
            return info;
        }

        MSPTInfo &getMSPTinfo() {
            static MSPTInfo info;
            return info;
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
