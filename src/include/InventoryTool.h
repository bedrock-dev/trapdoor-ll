//
// Created by xhy on 2022/6/29.
//

#ifndef TRAPDOOR_INVENTORY_TOOL_H
#define TRAPDOOR_INVENTORY_TOOL_H
#include <mc/BlockInstance.hpp>
#include <mc/Inventory.hpp>
#include <mc/Player.hpp>
namespace trapdoor {
    bool onStartDestroyBlock(Player *player, const BlockInstance &instance);

    void swapItemInContainer(Container &cont, int s1, int s2);

}  // namespace trapdoor

#endif  // TRAPDOOR_INVENTORY_TOOL_H
