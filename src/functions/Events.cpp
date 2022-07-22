//
// Created by xhy on 2022/6/15.
//
#include "Events.h"

#include <MC/Block.hpp>
#include <MC/Item.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Level.hpp>

#include "BlockRotateHelper.h"
#include "EventAPI.h"
#include "Global.h"
#include "InventoryTool.h"
#include "Shortcuts.h"
#include "TrapdoorMod.h"
#include "Utils.h"

namespace trapdoor {
    namespace {
        // 右键消除抖动
        struct UseOnAction {
            uint64_t gameTick = 0;
            BlockPos pos;
            bool operator==(const UseOnAction& rhs) const {
                if (pos != rhs.pos) return false;
                return (gameTick - rhs.gameTick) <= 3;
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
                // getUseOnCache()[playerName] = useOnAction;
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
            auto& shortcuts = trapdoor::mod().getConfig().getShortcuts();
            if (shortcuts.empty()) {
                return true;
            }
            Shortcut shortcut;
            shortcut.type = USE;
            shortcut.itemAux = ev.mItemStack->getAux();
            shortcut.itemName = trapdoor::rmmc(ev.mItemStack->getTypeName());
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
            if (ev.mItemStack->getTypeName() == "minecraft:cactus" &&
                antiShake(ev.mPlayer->getName(), bi->getPosition())) {
                trapdoor::rotateBlock(bi->getBlockSource(), bi, ev.mClickPos, ev.mFace);
                return true;
            }

            auto& shortcuts = trapdoor::mod().getConfig().getShortcuts();
            if (shortcuts.empty()) {
                return true;
            }

            Shortcut shortcut;
            shortcut.type = USE_ON;
            shortcut.itemAux = ev.mItemStack->getAux();
            shortcut.itemName = trapdoor::rmmc(ev.mItemStack->getTypeName());
            shortcut.blockAux = block->getVariant();
            shortcut.blockName = trapdoor::rmmc(block->getName().getString());
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
            trapdoor::mod().getSimPlayerManager().processDieEvent(ev.mPlayer->getRealName());
            return true;
        });
    }

    void subscribePlayerStartDestroyBlockEvent() {
        Event::PlayerStartDestroyBlockEvent::subscribe(
            [&](const Event::PlayerStartDestroyBlockEvent& ev) {
                return onStartDestroyBlock(ev.mPlayer, ev.mBlockInstance);
            });
    }

    void subscribePlayerPlaceBlockEvent() {}
    void subscribePlayerInventoryChangeEvent() {
        Event::PlayerInventoryChangeEvent::subscribe(
            [&](const Event::PlayerInventoryChangeEvent& ev) {
                trapdoor::mod().getSimPlayerManager().tryRefreshInv(ev.mPlayer);
                return true;
            });
    }
}  // namespace trapdoor

// TInstanceHook(bool,
// "?_useOn@BlockItem@@MEBA_NAEAVItemStack@@AEAVActor@@VBlockPos@@EAEBVVec3@@@Z",
//               Item, ItemStack* a2, Actor* ac, BlockPos* a4, unsigned __int8 a5, class Vec3* a6) {
//     auto id = a2 ? a2->getId() : -1024;
//     auto res = original(this, a2, ac, a4, a5, a6);
//     if (ac && ac->isPlayer()) {
//         trapdoor::afterUseItem((Player*)(ac), static_cast<int>(id));
//     }
//     return res;
// }
