//
// Created by xhy on 2022/8/12.
//

#include "EventTrigger.h"

namespace trapdoor {

    namespace {

        const std::unordered_map<std::string, TrEventType>& eventMap() {
            static std::unordered_map<std::string, TrEventType> map = {
                {"signalchanged", SignalChanged}, {"tntexplode", TNTExplode}};
            return map;
        }

        std::string trEventToStr(TrEventType type) {
            switch (type) {
                case SignalChanged:
                    return "signal change";
                case TNTExplode:
                    return "TNT explode";
            }
            return "Unknown";
        }

    }  // namespace

    std::vector<std::string> collectTrEventsStr() {
        std::vector<std::string> res;
        for (auto& kv : eventMap()) {
            res.push_back(kv.first);
        }
        return res;
    }

    ActionResult EventTriggerManager::listEvents(const std::string& name) const {
        std::string res;
        for (auto& ev : this->subscribeInfo) {
            if (ev.second.count(name)) {
                res += trEventToStr(ev.first);
                res += "\n";
            }
        }
        return {res, true};
    }

    ActionResult EventTriggerManager::eventAction(const std::string& name,
                                                  const std::string& eventType, int action) {
        auto it = eventMap().find(eventType);
        if (it == eventMap().end()) {
            return {"Error event", false};
        }
        auto type = it->second;
        if (action == 0) {
            this->subscribe(name, type);
        } else {
            this->unsubscribe(name, type);
        }
    }
}  // namespace trapdoor
