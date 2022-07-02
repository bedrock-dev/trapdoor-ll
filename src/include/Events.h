#ifndef TRAPDOOR_EVENT_H
#define TRAPDOOR_EVENT_H

namespace tr {
    void subscribeItemUseOnEvent();
    void subscribeItemUseEvent();
    void subscribePlayerDieEvent();
    void subscribePlayerStartDestroyBlockEvent();
    void subscribePlayerPlaceBlockEvent();

    inline void SubscribeEvents() {
        subscribeItemUseOnEvent();
        subscribeItemUseEvent();
        subscribePlayerDieEvent();
        subscribePlayerStartDestroyBlockEvent();
        subscribePlayerPlaceBlockEvent();
    }

}  // namespace tr

#endif
