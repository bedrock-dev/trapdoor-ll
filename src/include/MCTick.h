#ifndef _TRAPDOOR_GAME_TICK_H_
#define _TRAPDOOR_GAME_TICK_H_

namespace tr {
    enum class TickingStatus {
        Normal, Forwarding, Slowing, Frozen, Acc
    };

    double getMeanMSPT();

    double getMeanTPS();

} // namespace tr
#endif
