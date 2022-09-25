//
// Created by xhy on 2022/8/12.
//

#include "EventTrigger.h"

#include <mc/Level.hpp>

#include "Global.h"
#include "Msg.h"
namespace trapdoor {

    namespace {

        const std::unordered_map<std::string, TrEventType>& eventMap() {
            static std::unordered_map<std::string, TrEventType> map = {
                {"signalchange", SignalChange},
                {"entityexplode", EntityExplode},
                {"buildconn", BuildConnection}};
            return map;
        }

        std::string trEventToStr(TrEventType type) {
            switch (type) {
                case SignalChange:
                    return "signal change";
                case EntityExplode:
                    return "TNT explode";
                case BuildConnection:
                    return "Build connection";
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
        return {"Success", true};
    }
    void EventTriggerManager::broadcastMessage(TrEventType eventType,
                                               const std::string& msg) const {
        auto it = this->subscribeInfo.find(eventType);
        if (it == this->subscribeInfo.end()) {
            return;
        }

        trapdoor::TextBuilder b;
        b.sTextF(TB::AQUA | TB::BOLD, "[%zu] ", Global<Level>->getCurrentServerTick().t % 72000);
        b.textF("%s", msg.c_str());
        for (auto& name : it->second) {
            auto* p = Global<Level>->getPlayer(name);
            if (p) {
                p->sendText(b.get());
            }
        }
    }
}  // namespace trapdoor
