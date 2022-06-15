//
// Created by xhy on 2022/6/15.
//
#include "Events.h"

#include <MC/Block.hpp>
#include <MC/ItemStack.hpp>

#include "EventAPI.h"
#include "Shortcuts.h"
#include "TrapdoorMod.h"
#include "Utils.h"

namespace tr {
    void subscribeItemUseEvent() {
        Event::PlayerUseItemEvent::subscribe([&](const Event::PlayerUseItemEvent& ev) {
            auto& shortcuts = tr::mod().getConfig().getShortcuts();
            if (shortcuts.empty()) {
                return true;
            }
            Shortcut shortcut;
            shortcut.type = USE;
            shortcut.itemAux = ev.mItemStack->getAux();
            shortcut.itemName = tr::rmmc(ev.mItemStack->getTypeName());
            for (auto& sh : shortcuts) {
                if (sh.match(shortcut)) {
                    sh.runUse(ev.mPlayer, ev.mItemStack);
                    return !sh.prevent;
                }
            }

            return true;
        });
    }

    void subscribeItemUseOnEvent() {
        Event::PlayerUseItemOnEvent::subscribe([&](const Event::PlayerUseItemOnEvent& ev) {
            auto& shortcuts = tr::mod().getConfig().getShortcuts();
            if (shortcuts.empty()) {
                return true;
            }
            auto* bi = const_cast<BlockInstance*>(&ev.mBlockInstance);
            if (bi->isNull()) {
                return true;
            }
            auto* block = bi->getBlock();
            Shortcut shortcut;
            shortcut.type = USE_ON;
            shortcut.itemAux = ev.mItemStack->getAux();
            shortcut.itemName = tr::rmmc(ev.mItemStack->getTypeName());
            shortcut.blockAux = block->getVariant();
            shortcut.blockName = tr::rmmc(block->getName().getString());
            tr::logger().debug("USE ON HOOK:   ", shortcut.getDescription());
            for (auto sh : shortcuts) {
                if (sh.match(shortcut)) {
                    sh.runUseOn(ev.mPlayer, ev.mItemStack, block, bi->getPosition());
                    return !sh.prevent;
                }
            }
            return true;
        });
    }
}  // namespace tr
