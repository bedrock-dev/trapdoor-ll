//
// Created by xhy on 2022/6/15.
//
#include "Events.h"

#include <MC/Block.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Level.hpp>

#include "BlockRotateHelper.h"
#include "EventAPI.h"
#include "Global.h"
#include "InventoryTool.h"
#include "Shortcuts.h"
#include "TrapdoorMod.h"
#include "Utils.h"

namespace tr {

    namespace {
        // 右键消除抖动
        struct UseOnAction {
            uint64_t gameTick = 0;
            BlockPos pos;
            bool operator==(const UseOnAction& rhs) const {
                if (pos != rhs.pos) return false;
                return (gameTick - rhs.gameTick) <= 4;
            }
            bool operator!=(const UseOnAction& rhs) const { return !(rhs == *this); }
        };

        std::unordered_map<std::string, UseOnAction>& getUseOnCache() {
            static std::unordered_map<std::string, UseOnAction> cache;
            return cache;
        }

        bool antiShake(const std::string& playerName, const BlockPos& pos) {
            uint64_t gt = Global<Level>->getCurrentServerTick().t;
            auto useOnAction = UseOnAction{gt, pos};
            auto lastUseOnAction = getUseOnCache()[playerName];
            if (useOnAction == lastUseOnAction) {
                getUseOnCache()[playerName] = useOnAction;
                return false;
            }
            getUseOnCache()[playerName] = useOnAction;
            return true;
        }

        std::unordered_map<std::string, bool> getDisableDestroyMap() {
            static std::unordered_map<std::string, bool> cache;
            return cache;
        }

    }  // namespace

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
            auto* bi = const_cast<BlockInstance*>(&ev.mBlockInstance);
            if (bi->isNull()) {
                return true;
            }
            auto* block = bi->getBlock();
            //            if (ev.mItemStack->getName() == "Cactus" &&
            //                antiShake(ev.mPlayer->getName(), bi->getPosition())) {
            //                tr::rotateBlock(ev.mPlayer->getRegion(), bi->getPosition());
            //                return true;
            //            }

            auto& shortcuts = tr::mod().getConfig().getShortcuts();
            if (shortcuts.empty()) {
                return true;
            }
            Shortcut shortcut;
            shortcut.type = USE_ON;
            shortcut.itemAux = ev.mItemStack->getAux();
            shortcut.itemName = tr::rmmc(ev.mItemStack->getTypeName());
            shortcut.blockAux = block->getVariant();
            shortcut.blockName = tr::rmmc(block->getName().getString());
            for (auto sh : shortcuts) {
                if (sh.match(shortcut)) {
                    if (antiShake(ev.mPlayer->getRealName(), bi->getPosition())) {
                        sh.runUseOn(ev.mPlayer, ev.mItemStack, block, bi->getPosition());
                        return !sh.prevent;
                    }
                }
            }
            return true;
        });
    }

    void subscribePlayerDieEvent() {
        Event::PlayerDieEvent::subscribe([&](const Event::PlayerDieEvent& ev) {
            tr::mod().getSimPlayerManager().processDieEvent(ev.mPlayer->getRealName());
            return true;
        });
    }

    void subscribePlayerStartDestroyBlockEvent() {
        Event::PlayerStartDestroyBlockEvent::subscribe(
            [&](const Event::PlayerStartDestroyBlockEvent& ev) {
                return onStartDestroyBlock(ev.mPlayer, ev.mBlockInstance);
            });
    }

    void subscribePlayerDestroyBlockEvent() {

        Event::PlayerDestroyBlockEvent::subscribe([&](const Event::PlayerDestroyBlockEvent& ev) {
            // 没耐久了阻止方块挖掘
            return true;
        });
    }
}  // namespace tr
