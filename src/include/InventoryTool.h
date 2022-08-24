//
// Created by xhy on 2022/6/29.
//

#ifndef TRAPDOOR_INVENTORY_TOOL_H
#define TRAPDOOR_INVENTORY_TOOL_H
#include <MC/BlockInstance.hpp>
#include <MC/Inventory.hpp>
#include <MC/Player.hpp>
namespace trapdoor {
    bool onStartDestroyBlock(Player *player, const BlockInstance &instance);

    void swapItemInContainer(Container &cont, int s1, int s2);

}  // namespace trapdoor

#endif  // TRAPDOOR_INVENTORY_TOOL_H
