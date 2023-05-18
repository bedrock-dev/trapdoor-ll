//
// Created by xhy on 2022/6/15.
//
#include "Events.h"

#include <mc/Block.hpp>
#include <mc/Item.hpp>
#include <mc/ItemStack.hpp>
#include <mc/Level.hpp>

#include "BlockRotateHelper.h"
#include "EventAPI.h"
#include "Global.h"
#include "InventoryTool.h"
#include "Msg.h"
#include "Shortcuts.h"
#include "TVec3.h"
#include "TrapdoorMod.h"
#include "Utils.h"

namespace trapdoor {
    namespace {
        // 右键消除抖动
        struct UseOnAction {
            uint64_t gameTick = 0;
            BlockPos pos;

            bool operator==(const UseOnAction &rhs) const {
                if (pos != rhs.pos) return false;
                return (gameTick - rhs.gameTick) <= 3;
            }

            bool operator!=(const UseOnAction &rhs) const { return !(rhs == *this); }
        };

        std::unordered_map<std::string, UseOnAction> &getUseOnCache() {
            static std::unordered_map<std::string, UseOnAction> cache;
            return cache;
        }

        bool antiShake(const std::string &playerName, const BlockPos &pos) {
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

        // void resetAntiShakeCache(const std::string &name) { getUseOnCache().erase(name); }

    }  // namespace

    void subscribeItemUseEvent() {
        Event::PlayerUseItemEvent::subscribe([&](const Event::PlayerUseItemEvent &ev) {
            auto &shortcuts = trapdoor::mod().getConfig().getShortcuts();
            if (shortcuts.empty()) {
                return true;
            }

            Shortcut shortcut;
            shortcut.type = USE;
            shortcut.itemAux = ev.mItemStack->getAux();
            shortcut.itemName = trapdoor::rmmc(ev.mItemStack->getTypeName());
            for (auto &kv : shortcuts) {
                if (kv.second.enable && kv.second.match(shortcut)) {
                    trapdoor::logger().debug("trigger: {}", kv.second.getDescription());
                    kv.second.runUse(ev.mPlayer, ev.mItemStack);
                    return !kv.second.prevent;
                }
            }

            return true;
        });
    }

    void subscribeItemUseOnEvent() {
        Event::PlayerUseItemOnEvent::subscribe([&](const Event::PlayerUseItemOnEvent &ev) {
            auto *bi = const_cast<BlockInstance *>(&ev.mBlockInstance);
            if (bi->isNull()) {
                return true;
            }

            auto *block = bi->getBlock();

            auto &shortcuts = trapdoor::mod().getConfig().getShortcuts();
            Shortcut shortcut;
            shortcut.type = USE_ON;
            shortcut.itemAux = ev.mItemStack->getAux();
            shortcut.itemName = trapdoor::rmmc(ev.mItemStack->getTypeName());
            shortcut.blockAux = block->getVariant();
            shortcut.blockName = trapdoor::rmmc(block->getName().getString());
            for (auto kv : shortcuts) {
                if (kv.second.enable && kv.second.match(shortcut)) {
                    if (antiShake(ev.mPlayer->getRealName(), bi->getPosition())) {
                        trapdoor::logger().debug("Trigger event: {}", kv.second.getDescription());
                        kv.second.runUseOn(ev.mPlayer, ev.mItemStack, block, bi->getPosition());
                        return !kv.second.prevent;
                    }
                }
            }
            if (ev.mItemStack->getTypeName() == "minecraft:cactus" &&
                antiShake(ev.mPlayer->getRealName(), bi->getPosition())) {
                trapdoor::rotateBlock(ev.mPlayer, bi->getBlockSource(), bi, ev.mClickPos, ev.mFace);
            }
            return true;
        });
    }

    void subscribePlayerDieEvent() {
        Event::PlayerDieEvent::subscribe([&](const Event::PlayerDieEvent &ev) {
            trapdoor::mod().getSimPlayerManager().processDieEvent(ev.mPlayer->getRealName());
            return true;
        });
    }

    void subscribePlayerStartDestroyBlockEvent() {
        Event::PlayerDestroyBlockEvent::subscribe([&](const Event::PlayerDestroyBlockEvent &ev) {
            auto *bi = const_cast<BlockInstance *>(&ev.mBlockInstance);
            if (bi->isNull()) {
                return true;
            }
            auto *block = bi->getBlock();
            Shortcut shortcut;
            shortcut.type = DESTROY;
            shortcut.itemAux = ev.mPlayer->getSelectedItem().getAux();
            shortcut.itemName = trapdoor::rmmc(ev.mPlayer->getSelectedItem().getTypeName());

            shortcut.blockAux = block->getVariant();
            shortcut.blockName = trapdoor::rmmc(block->getName().getString());
            auto &shortcuts = trapdoor::mod().getConfig().getShortcuts();
            for (auto &kv : shortcuts) {
                if (kv.second.enable && kv.second.match(shortcut)) {
                    kv.second.runUseDestroy(ev.mPlayer, &ev.mPlayer->getSelectedItem(), block,
                                            bi->getPosition());
                    return !kv.second.prevent;
                }
            }

            return true;
        });

        Event::PlayerStartDestroyBlockEvent::subscribe(
            [&](const Event::PlayerStartDestroyBlockEvent &ev) {
                return onStartDestroyBlock(ev.mPlayer, ev.mBlockInstance);
            });
    }

    void subscribePlayerPlaceBlockEvent() {}

    void subscribeServerStartEvent() {
        Event::ServerStartedEvent::subscribe([&](const Event::ServerStartedEvent &ev) {
            // trapdoor::mod().getSimPlayerManager().addPlayersInCache();
            return true;
        });

        Event::EntityExplodeEvent::subscribe([&](const Event::EntityExplodeEvent &ev) {
            trapdoor::TextBuilder builder;
            auto p = ev.mPos;
            builder
                .sTextF(TB::BOLD | TB::WHITE, "%s",
                        trapdoor::rmmc(ev.mActor->getTypeName()).c_str())
                .text(" explode at ")
                .sTextF(TB::WHITE, "[%.5f, %.5f,  %.5f]", p.x, p.y, p.z)
                .text(" with radius ")
                .num(ev.mRadius);
            trapdoor::mod().getEventTriggerMgr().broadcastMessage(EntityExplode, builder.get());
            return true;
        });
    }

}  // namespace trapdoor

// TR event
// TODO: MOVE TO A SINGLE CLASS
#include <mc/BaseCircuitComponent.hpp>
#include <mc/CircuitSystem.hpp>
#include <mc/ConsumerComponent.hpp>

#include "HookAPI.h"
// THook(bool, "?evaluate@ConsumerComponent@@UEAA_NAEAVCircuitSystem@@AEBVBlockPos@@@Z",
//       ConsumerComponent* comp, CircuitSystem& sys, const BlockPos& pos) {
//     auto o = comp->getStrength();
//     auto res = original(comp, sys, pos);
//     auto n = comp->getStrength();
//
//     if (o != n) {
//         trapdoor::TextBuilder builder;
//         builder.sTextF(trapdoor::TB::WHITE, "[%d, %d ,%d]  ", pos.x, pos.y, pos.z)
//             .sTextF(trapdoor::TB::WHITE | trapdoor::TB::BOLD, "%d", o)
//             .text(" -> ")
//             .sTextF(trapdoor::TB::RED | trapdoor::TB::BOLD, "%d", n);
//         trapdoor::mod().getEventTriggerMgr().broadcastMessage(trapdoor::SignalChange,
//                                                               builder.get());
//     }
//     return res;
// }

// THook(std::string*,
//       "?getWorldsPath@FilePathManager@Core@@QEBA?AV?$PathBuffer@V?$basic_string@DU?$char_traits@D@"
//       "std@@V?$allocator@D@2@@std@@@2@XZ",
//       void* self, void* a, void* b) {
//     auto r = original(self, a, b);
//     *r = std::string(
//         "C:/Users/xhy/AppData/Local/Packages/Microsoft.MinecraftUWP_8wekyb3d8bbwe/LocalState/games/"
//         "com.mojang/minecraftWorlds/");
//     return r;
// }