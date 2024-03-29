//
// Created by xhy on 2022/8/12.
//

#ifndef TRAPDOOR_EVENT_TRIGGER_H
#define TRAPDOOR_EVENT_TRIGGER_H
#include <unordered_map>
#include <unordered_set>

#include "CommandHelper.h"

namespace trapdoor {

    std::vector<std::string> collectTrEventsStr();

    enum TrEventType : int { SignalChange, EntityExplode, BuildConnection };
    class EventTriggerManager {
       public:
        inline void subscribe(const std::string& name, TrEventType type) {
            this->subscribeInfo[type].insert(name);
        }

        inline void unsubscribe(const std::string& name, TrEventType type) {
            this->subscribeInfo[type].erase(name);
        }

        void broadcastMessage(TrEventType eventType, const std::string& msg) const;
        ActionResult eventAction(const std::string& name, const std::string& eventName, int action);

        [[nodiscard]] ActionResult listEvents(const std::string& name) const;

       private:
        std::unordered_map<TrEventType, std::unordered_set<std::string>> subscribeInfo;
    };

}  // namespace trapdoor

class EventTrigger {};
#endif  // TRAPDOOR_EVENT_TRIGGER_H
