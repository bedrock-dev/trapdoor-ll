#ifndef TRAPDOOR_EVENT_H
#define TRAPDOOR_EVENT_H

namespace trapdoor {
    void subscribeItemUseOnEvent();
    void subscribeItemUseEvent();
    void subscribePlayerDieEvent();
    void subscribePlayerStartDestroyBlockEvent();
    void subscribePlayerPlaceBlockEvent();

    void subscribeServerStartEvent();

    inline void SubscribeEvents() {
        subscribeItemUseOnEvent();
        subscribeItemUseEvent();
        subscribePlayerDieEvent();
        subscribePlayerStartDestroyBlockEvent();
        subscribePlayerPlaceBlockEvent();
        subscribeServerStartEvent();
    }

}  // namespace trapdoor

#endif
