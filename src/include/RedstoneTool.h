//
// Created by xhy on 2022/8/26.
//

#ifndef TRAPDOOR_REDSTONE_TOOL_H
#define TRAPDOOR_REDSTONE_TOOL_H
#include <mc/BlockPos.hpp>
#include <mc/Dimension.hpp>
#include <string>

#include "CommandHelper.h"
namespace trapdoor {

    ActionResult displayRedstoneCompInfo(Dimension *d, const BlockPos &pos,
                                         const std::string &type);

}  // namespace trapdoor

#endif  // TRAPDOOR_REDSTONETOOL_H
