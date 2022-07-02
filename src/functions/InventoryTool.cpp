//
// Created by xhy on 2022/6/29.
//

#include "InventoryTool.h"

#include <MC/Block.hpp>
#include <MC/Inventory.hpp>
#include <MC/ItemStack.hpp>
#include <MC/Material.hpp>
#include <cstdio>

#include "Config.h"
#include "Msg.h"
#include "TrapdoorMod.h"
#include "Utils.h"
namespace tr {

    namespace {

        struct ToolInfo {
            float speed;
            int slot;
            short remainDamage;
        };

        int searchBestToolInInv(Container &inv, int currentSlot, const Block *b) {
            auto curItem = inv.getSlot(currentSlot);
            ToolInfo curInfo{curItem->getDestroySpeed(*b), currentSlot,
                             (short)(curItem->getMaxDamage() - curItem->getDamageValue())};
            auto sz = inv.getSize();
            for (int i = 0; i < sz; i++) {
                auto *item = inv.getSlot(i);
                if (item && item->getCount() != 0) {
                    auto speed = item->getDestroySpeed(*b);
                    auto remainDamage = item->getMaxDamage() - item->getDamageValue();
                    if (remainDamage <= 5) continue;
                    if (speed > curInfo.speed) {
                        curInfo = {speed, i, (short)remainDamage};
                    }
                }
            }

            return curInfo.slot;
        }
    }  // namespace

    void swapItemInContainer(Container &cont, int s1, int s2) {
        auto i1 = cont.getItem(s1).clone();
        auto i2 = cont.getItem(s2).clone();
        cont.removeItem(s1, 64);
        cont.removeItem(s2, 64);
        cont.setItem(s1, i2);
        cont.setItem(s2, i1);
    }
    bool onStartDestroyBlock(Player *player, const BlockInstance &instance) {
        if (!tr::mod().getConfig().getTweakConfig().autoSelectTool) {
            return true;
        }
        auto *ins = const_cast<BlockInstance *>(&instance);
        if (!player || !ins || ins->isNull()) return true;
        auto curSlot = player->getSelectedItemSlot();
        auto bestSlot = searchBestToolInInv(player->getInventory(), curSlot, ins->getBlock());
        if (bestSlot == curSlot) {
            auto &item = player->getSelectedItem();
            auto remain = item.getMaxDamage() - item.getDamageValue();
            if (remain <= 5) {
                TextBuilder builder;
                builder.sText(TB::GOLD, " You don't have the right tools!");
                player->sendText(builder.get());
                return false;
            } else {
                return true;
            }
        }

        if (bestSlot <= 8) {
            player->setSelectedSlot(bestSlot);
        } else {
            swapItemInContainer(player->getInventory(), curSlot, bestSlot);
        }
        return true;
    }


}  // namespace tr
