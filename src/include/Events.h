#ifndef TRAPDOOR_EVENT_H
#define TRAPDOOR_EVENT_H

namespace tr {
    void subscribeItemUseOnEvent();
    void subscribeItemUseEvent();

    inline void SubscribeEvents() {
        subscribeItemUseOnEvent();
        subscribeItemUseEvent();
    }

}  // namespace tr

#endif
