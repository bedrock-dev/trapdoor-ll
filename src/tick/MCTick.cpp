#include "MCTick.h"
#include "HookAPI.h"
#include "LoggerAPI.h"
#include "TrapdoorMod.h"

namespace tr {
    namespace {
        TickingStatus ticking_status = TickingStatus::Normal;
    }

    void freeze_world() { ticking_status = TickingStatus::Frozen; }

} // namespace tr

THook(void, "?tick@ServerLevel@@UEAAXXZ", void *level) {
    return original(level);
}