//
// Created by xhy on 2022/6/29.
//

#ifndef TRAPDOOR_AUTOMATICTOOL_H
#define TRAPDOOR_AUTOMATICTOOL_H
#include <MC/BlockInstance.hpp>
#include <MC/Player.hpp>
namespace tr {

    bool onStartDestroyBlock(Player *player, const BlockInstance &instance);

    void onDestroyBlock();

}  // namespace tr

#endif  // TRAPDOOR_AUTOMATICTOOL_H
