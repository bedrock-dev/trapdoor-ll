#ifndef TRAPDOOR_EVENT_H
#define TRAPDOOR_EVENT_H

namespace tr {
    void SubscribeItemUseOnEvent();
    void SubscribePlayerAttackActorEvent();

    inline void SubscribeEvents() {
        SubscribePlayerAttackActorEvent();
        SubscribeItemUseOnEvent();
    }
}  // namespace tr

#endif
