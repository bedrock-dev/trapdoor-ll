#include "MCTick.h"

#include <chrono>

#include "HookAPI.h"
#include "LoggerAPI.h"
#include "ProfileInfo.h"
#include "TrapdoorMod.h"

namespace tr {
    namespace {
        struct TickingParam {
            size_t slow_dome_time;
            size_t forward_tick;
            size_t acc_tile;
            TickingStatus status = TickingStatus::Normal;
        };

        TickingStatus TICKING_STATUS = TickingStatus::Normal;

        MSPTInfo &getMSPTinfo() {
            static MSPTInfo info;
            return info;
        }

    }  // namespace

    void freeze_world() { TICKING_STATUS = TickingStatus::Frozen; }

    double getMeanMSPT() { return tr::micro_to_mill(getMSPTinfo().mean()); }

    double getMeanTPS() {
        auto tps = 1000.0 / tr::getMeanMSPT();
        return tps > 20.0 ? 20.0 : tps;
    }

}  // namespace tr

THook(void, "?tick@ServerLevel@@UEAAXXZ", void *level) {
    TIMER_START
    original(level);
    TIMER_END
    tr::getMSPTinfo().push(timeReslut);

    switch (tr::TICKING_STATUS) {
        case tr::TickingStatus::Normal:
            break;
        case tr::TickingStatus::Forwarding:
            break;
        case tr::TickingStatus::Slowing:
            break;
        case tr::TickingStatus::Frozen:
            break;
        case tr::TickingStatus::Acc:
            break;
    }
}
