//
// Created by xhy on 2022/7/19.
//

#ifndef TRAPDOOR_DISTANCEHELPER_H
#define TRAPDOOR_DISTANCEHELPER_H
#include <MC/Player.hpp>

#include "CommandHelper.h"
namespace trapdoor {

    ActionResult setFirstPos(Player *player, const BlockPos &p1);

    ActionResult getDistance(Player *player, const BlockPos &p1, const BlockPos &p2);

}  // namespace trapdoor

#endif  // TRAPDOOR_DISTANCEHELPER_H
