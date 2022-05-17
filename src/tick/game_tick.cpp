#include "game_tick.h"
#include "HookAPI.h"
namespace tr {
namespace {
TickingStatus ticking_status = TickingStatus::Normal;
}

void freeze_world() { ticking_status = TickingStatus::Frozen; }

// THook("12", void *level, int tick) {
// }

} // namespace tr
