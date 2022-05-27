
#include <functional>
#include <map>
#include <string>
#include <unordered_map>

#include "EventAPI.h"
#include "InfoDisplayer.h"
#include "TrapdoorMod.h"
#include "VillageHelper.h"

namespace tr {

    namespace {

        // std::map<std::string, std::function<bool(const std::string,
        // Actor*)>>& attackEventdsMap() {

        // }

    }  // namespace

    void SubscribePlayerAttackActorEvent() {
        Event::PlayerAttackEvent::subscribe(
            [](const Event::PlayerAttackEvent& ev) {
                tr::logger().debug("player {} attack {}",
                                   ev.mPlayer->getRealName(),
                                   ev.mTarget->getTypeName());
                if (ev.mTarget->getTypeName() == "minecraft:villager_v2") {
                    return tr::mod().village_helper().ShowVillageInfo(
                        ev.mPlayer, ev.mTarget);
                }

                return tr::displayEntityInfo(ev.mPlayer, ev.mTarget);
                ;
            });
    }
}  // namespace tr