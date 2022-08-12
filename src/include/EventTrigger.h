//
// Created by xhy on 2022/8/12.
//

#ifndef TRAPDOOR_EVENTTRIGGER_H
#define TRAPDOOR_EVENTTRIGGER_H
#include <unordered_map>
#include <unordered_set>

#include "CommandHelper.h"

namespace trapdoor {

    std::vector<std::string> collectTrEventsStr();

    enum TrEventType : int { SignalChanged, TNTExplode };
    class EventTriggerManager {
       public:
        inline void subscribe(const std::string& name, TrEventType type) {
            this->subscribeInfo[type].insert(name);
        }

        inline void unsubscribe(const std::string& name, TrEventType type) {
            this->subscribeInfo[type].erase(name);
        }

        ActionResult eventAction(const std::string& name, const std::string& eventName, int action);

        ActionResult listEvents(const std::string& name) const;

       private:
        std::unordered_map<TrEventType, std::unordered_set<std::string>> subscribeInfo;
    };

}  // namespace trapdoor

class EventTrigger {};
#endif  // TRAPDOOR_EVENTTRIGGER_H
