#include <MC/Block.hpp>
#include <MC/BlockInstance.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Player.hpp>

#include "EventAPI.h"
#include "Events.h"
#include "InfoDisplay.h"
#include "TrapdoorMod.h"

namespace tr {

    void SubscribeItemUseOnEvent() {
        Event::PlayerUseItemOnEvent ::subscribe(
            [](const Event::PlayerUseItemOnEvent& ev) {
                // auto* bi = const_cast<BlockInstance*>(&ev.mBlockInstance);
                // tr::displayBlockInfo(ev.mPlayer, bi);
                // return true;
                return true;
            });

        // Event::PlayerAttackEvent::subscribe(
        //     [](const Event::PlayerAttackEvent& ev) {
        //         tr::logger().debug("player {} attack {}",
        //                            ev.mPlayer->getRealName(),
        //                            ev.mTarget->getTypeName());
        //         return false;
        //     });

        // Event::PlayerUseItemEvent ::subscribe(
        //     [](const Event::PlayerUseItemEvent& ev) { return true; });
    }

}  // namespace tr
