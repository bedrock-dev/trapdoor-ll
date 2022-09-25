//
// Created by xhy on 2022/7/19.
//

#ifndef TRAPDOOR_DISTANCE_HELPER_H
#define TRAPDOOR_DISTANCE_HELPER_H
#include <mc/Player.hpp>

#include "CommandHelper.h"
namespace trapdoor {

    ActionResult setFirstPos(Player *player, const BlockPos &p1);

    ActionResult getDistance(Player *player, const BlockPos &p1, const BlockPos &p2);

}  // namespace trapdoor

#endif  // TRAPDOOR_DISTANCE_HELPER_H
