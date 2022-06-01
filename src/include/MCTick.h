#ifndef TRAPDOOR_GAME_TICK_H
#define TRAPDOOR_GAME_TICK_H
#include "SimpleProfiler.h"
namespace tr {

    struct ActionResult;

    enum class TickingStatus {
        Normal,
        Forwarding,
        SlowDown,
        Frozen,
        Acc,
        Wrap
    };

    struct TickingInfo {
        int remain_wrap_tick = 0;
        size_t slow_down_time = 1;
        size_t forward_tick_num = 0;
        size_t acc_time = 1;
        size_t slow_down_counter = 0;
        TickingStatus status = TickingStatus::Normal;
        TickingStatus old_status = TickingStatus::Normal;
    };

    // Log Commmand action
    double getMeanMSPT();

    double getMeanTPS();

    ActionResult PrintMspt();
    // Tick Command action
    ActionResult FreezeWorld();

    ActionResult ResetWorld();

    ActionResult ForwardWorld(int gt);

    ActionResult SlowDownWorld(int times);

    ActionResult AccWorld(int times);

    ActionResult WrapWorld(int times);

    ActionResult QueryWorld();

    ActionResult CancelWorld();

    ActionResult StartProfiler(int rounds, SimpleProfiler::Type type);

}  // namespace tr
#endif
