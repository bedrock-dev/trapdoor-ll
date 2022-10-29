//
// Created by xhy on 2022/10/5.
//

#ifndef TRAPDOOR_RANDOMTWEAKER_H
#define TRAPDOOR_RANDOMTWEAKER_H
#include <string>

#include "CommandHelper.h"
namespace trapdoor {
    /**
     * Fixed random sequence
     * @param type
     * @param value
     */
    ActionResult tweakFixedRandomNumber(const std::string& type, int value);

}  // namespace trapdoor

#endif  // TRAPDOOR_RANDOMTWEAKER_H
