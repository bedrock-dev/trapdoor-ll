#include "MCTick.h"
#include "HookAPI.h"
#include "LoggerAPI.h"
#include "TrapdoorMod.h"
#include "ProfileInfo.h"
#include <chrono>

namespace tr {
    namespace {
        TickingStatus ticking_status = TickingStatus::Normal;

        MSPTInfo &getMSPTinfo() {
            static MSPTInfo info;
            return info;
        }

    }

    void freeze_world() { ticking_status = TickingStatus::Frozen; }

    double getMeanMSPT() {
        return tr::micro_to_mill(getMSPTinfo().mean());
    }

    double getMeanTPS() {
        auto tps = 1000.0 / tr::getMeanMSPT();
        return tps > 20.0 ? 20.0 : tps;
    }

} // namespace tr


THook(void, "?tick@ServerLevel@@UEAAXXZ", void *level) {
    TIMER_START
    original(level);
    TIMER_END
    tr::getMSPTinfo().push(timeReslut);
}







