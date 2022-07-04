//
// Created by xhy on 2022/6/29.
//

#ifndef TRAPDOOR_INVENTORYTOOL_H
#define TRAPDOOR_INVENTORYTOOL_H
#include <MC/BlockInstance.hpp>
#include <MC/Inventory.hpp>
#include <MC/Player.hpp>
namespace tr {

    bool onStartDestroyBlock(Player *player, const BlockInstance &instance);

    void swapItemInContainer(Container &cont, int s1, int s2);

}  // namespace tr

#endif  // TRAPDOOR_INVENTORYTOOL_H
