#ifndef TRAPDOOR_EVENT_H
#define TRAPDOOR_EVENT_H

namespace trapdoor {
    void subscribeItemUseOnEvent();
    void subscribeItemUseEvent();
    void subscribePlayerDieEvent();
    void subscribePlayerStartDestroyBlockEvent();
    void subscribePlayerPlaceBlockEvent();
    void subscribePlayerInventoryChangeEvent();
    void subscribeServerStartEvent();

    inline void SubscribeEvents() {
        subscribeItemUseOnEvent();
        subscribeItemUseEvent();
        subscribePlayerDieEvent();
        subscribePlayerStartDestroyBlockEvent();
        subscribePlayerPlaceBlockEvent();
        subscribePlayerInventoryChangeEvent();
        subscribeServerStartEvent();
    }

}  // namespace trapdoor

#endif
