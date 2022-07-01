//
// Created by xhy on 2022/6/29.
//

#ifndef TRAPDOOR_INVENTORYTOOL_H
#define TRAPDOOR_INVENTORYTOOL_H
#include <MC/BlockInstance.hpp>
#include <MC/Player.hpp>
namespace tr {

    void onStartDestroyBlock(Player *player, const BlockInstance &instance);

    void onDestroyBlock();

}  // namespace tr

#endif  // TRAPDOOR_INVENTORYTOOL_H
