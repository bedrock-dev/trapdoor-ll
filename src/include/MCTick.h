#ifndef _TRAPDOOR_GAME_TICK_H_
#define _TRAPDOOR_GAME_TICK_H_

namespace tr {

    class ActionResult;

    enum class TickingStatus { Normal, Forwarding, SlowDown, Frozen, Acc };

    struct TickingInfo {
        size_t slow_down_time = 1;
        size_t forward_tick_num = 0;
        size_t acc_time = 1;
        size_t slow_down_counter = 0;
        TickingStatus status = TickingStatus::Normal;
    };

    // Log Commmand action
    double getMeanMSPT();

    double getMeanTPS();

    // Tick Command action
    ActionResult FreezeWorld();

    ActionResult ResetWorld();

    ActionResult ForwardWorld(int gt);

    ActionResult SlowDownWorld(int times);

    ActionResult AccWorld(int times);

}  // namespace tr
#endif
