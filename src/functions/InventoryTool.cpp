//
// Created by xhy on 2022/6/29.
//

#include "InventoryTool.h"

#include <cstdio>
#include <mc/Block.hpp>
#include <mc/Inventory.hpp>
#include <mc/ItemStack.hpp>
#include <mc/Material.hpp>

#include "Configuration.h"
#include "Msg.h"
#include "TrapdoorMod.h"
#include "Utils.h"

namespace trapdoor {
    namespace {

        struct ToolInfo {
            float speed;
            int slot;
            short remainDamage;
        };

        int searchBestToolInInv(Container &inv, int currentSlot, const Block *b) {
            auto curItem = inv.getSlot(currentSlot);
            ToolInfo curInfo{curItem->getDestroySpeed(*b), currentSlot,
                             (short) (curItem->getMaxDamage() - curItem->getDamageValue())};
            auto sz = inv.getSize();
            for (int i = 0; i < sz; i++) {
                auto *item = inv.getSlot(i);
                if (item && item->getCount() != 0) {
                    auto speed = item->getDestroySpeed(*b);
                    auto remainDamage = item->getMaxDamage() - item->getDamageValue();

                    // 跳过已经用坏的工具
                    if (remainDamage <=
                        trapdoor::mod().getConfig().getBasicConfig().toolDamageThreshold)
                        continue;
                    if (speed > curInfo.speed) {
                        curInfo = {speed, i, (short) remainDamage};
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
        if (!trapdoor::mod().getConfig().getGlobalFunctionConfig().autoSelectTool) {
            return true;
        }

        if (!player || !trapdoor::mod().getUserConfig().autotool(player->getRealName()) || player->isCreative()) {
            return true;
        }

        auto *ins = const_cast<BlockInstance *>(&instance);
        if (!ins || ins->isNull()) return true;
        auto curSlot = player->getSelectedItemSlot();
        auto bestSlot = searchBestToolInInv(player->getInventory(), curSlot, ins->getBlock());
        if (bestSlot == curSlot) {
            auto &item = player->getSelectedItem();
            auto remain = item.getMaxDamage() - item.getDamageValue();
            if (remain <= trapdoor::mod().getConfig().getBasicConfig().toolDamageThreshold) {
                TextBuilder builder;
                builder.sText(TB::GOLD, tr("tweak.autotool.no-tools"));
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

}  // namespace trapdoor
